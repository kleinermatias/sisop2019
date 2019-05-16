filepath = './statics'  
with open(filepath) as fp:  
   line = fp.readline()
   cnt = 1
   cuenta = 0
   linia = ''
   while line:
       linia = line.strip()
       print("Line {}: {}".format(cnt, line.strip()))
       line = fp.readline()
       cnt += 1
       cuenta = cuenta + int(linia)

print(cuenta/(cnt-1))
