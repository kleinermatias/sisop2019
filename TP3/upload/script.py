import subprocess,commands,time,threading,request,json,re,requests
from tqdm import tqdm

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def iniciar_onos():
    subprocess.Popen('cd $ONOS_ROOT && tools/build/onos-buck run onos-local -- clean debug',shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for i in tqdm(range(150)):
        time.sleep(0.4)
    return

def print_logo(): 
    print (bcolors.UNDERLINE + bcolors.HEADER + "Scrip ONOS 2019. Fulgor." + bcolors.ENDC)

def etapa_1(): #en esta estapa del script se buscan procesos onos, se los finaliza y se vuelve a iniciar el onos.
    tmp = subprocess.call('clear',shell=True)
    print_logo()
    print ("Buscando procesos de ONOS en ejecucion...")
    time.sleep(0.5)
    command = "ps -ef | grep onos | grep -v grep | awk '{print $2}'" #buscamos todos los procesos y separamos los referidos a ONOS, luego nos quedamos con el segundo argumento (PID)
    evaluar = commands.getstatusoutput(command)
    if "(0, '')" in str(evaluar): #significa que no hay procesos de ONOS corriendo, el comando retorna (0, "")
        print (bcolors.OKGREEN + "No se esta ejecutando ONOS, poniendo en ejecucion." + bcolors.ENDC)
        iniciar_onos()
    else: #Hay procesos onos, los elimino o los dejo?
        print (bcolors.WARNING + "Se encontraron uno o mas procesos referidos a ONOS." + bcolors.ENDC)
        opcion = raw_input("Eliminar procesos? Y/n \n")
        while (opcion != "Y") & (opcion != "y") & (opcion != "N") & (opcion != "n") :
            opcion = raw_input("Tecla incorrecta, Y/n \n")
        
        if (opcion=='Y') | (opcion=="y"):
            subprocess.check_output(command+" | xargs kill",shell=True)
            time.sleep(1)
            tmp = subprocess.call('clear',shell=True)
            print_logo()
            print("Procesos ONOS terminados, iniciando nuevamente..")
            iniciar_onos()

        else:
            tmp = subprocess.call('clear',shell=True)
            print_logo()
            print ("No se eliminan procesos ONOS.")


def etapa_2(): #cargo en ONOS mi app para APIS
    tmp = subprocess.call('clear',shell=True)
    print_logo()
    command = "cd ~/Escritorio/NETCONF-SDN/onos/app-altura/altura/ && mvn clean install && onos-app localhost reinstall! target/altura-1.0-SNAPSHOT.oar"
    evaluar = subprocess.call(command,shell=True)
    tmp = subprocess.call('clear',shell=True)
    print_logo()
    if evaluar==0:
        print (bcolors.OKBLUE + "Se cargo la app correctamente!" + bcolors.ENDC)
    else:
        print (bcolors.FAIL + "Ocurrio un error al cargar la app a ONOS" + bcolors.ENDC) 
    time.sleep(3)


def etapa_3(): #Cargo dockers a ONOS?
    tmp = subprocess.call('clear',shell=True)
    print_logo()
    print (bcolors.OKGREEN + "Desea cargar dockers de test al controlador ONOS? Y/n" + bcolors.ENDC)
    opcion=raw_input()
    while (opcion != "Y") & (opcion != "y") & (opcion != "N") & (opcion != "n") :
        opcion = raw_input("Tecla incorrecta, Y/n \n")
    if (opcion=='N') | (opcion=="n"):
            return
    else:
        print (bcolors.WARNING + "docker como sudo y servicio activado." + bcolors.ENDC)
        command = "docker pull matiasragnar/dockermxp:prueba"
        evaluar = subprocess.check_call(command,shell=True)

        command = "docker stop $(docker ps -a -q) && docker rm $(docker ps -a -q)"
        evaluar = subprocess.call(command,shell=True)
        
        command = "docker run -it -d matiasragnar/dockermxp:prueba"
        evaluar = subprocess.check_call(command,shell=True)
        command = "docker run -it -d matiasragnar/dockermxp:prueba"
        evaluar = subprocess.check_call(command,shell=True)

        data = {
            "devices": { 
            "netconf:"+"172.17.0.2"+":"+"830": {
                    "netconf": {
                        "ip": "172.17.0.2",
                        "port": 830,
                        "username": "root",
                        "password": "123",
                        "connect-timeout": 120,
                        "reply-timeout": 120,
                        "ssh-client": "ethz-ssh2"
                    },
                    "basic": {
                        "driver": "altura-netconf"
                    }
                }
            }
        }
        json_data = json.dumps(data)
        f = open ('holamundo.json','w')
        f.write(json_data)
        f.close()
        data = open('holamundo.json')
        headers = {
            'Accept': 'application/json',
        }

        response = requests.post('http://localhost:8181/onos/v1/network/configuration', headers=headers, data=data, auth=('onos', 'rocks'))


        data = {
            "devices": { 
            "netconf:"+"172.17.0.3"+":"+"830": {
                    "netconf": {
                        "ip": "172.17.0.3",
                        "port": 830,
                        "username": "root",
                        "password": "123",
                        "connect-timeout": 120,
                        "reply-timeout": 120,
                        "ssh-client": "ethz-ssh2"
                    },
                    "basic": {
                        "driver": "altura-netconf"
                    }
                }
            }
        }
        json_data = json.dumps(data)
        f = open ('holamundo.json','w')
        f.write(json_data)
        f.close()
        data = open('holamundo.json')
        headers = {
            'Accept': 'application/json',
        }

        response = requests.post('http://localhost:8181/onos/v1/network/configuration', headers=headers, data=data, auth=('onos', 'rocks'))


etapa_1()
etapa_2()
etapa_3()
var=input()