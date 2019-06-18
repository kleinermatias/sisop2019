// Definiendo __KERNEL__ and MODULE nos permite acceder a nivel de kernel.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE
// Linux Kernel/LKM headers: module.h es necesario para todos los modulos y kernel.h 
//y tambien para para KERN_INFO.

#include <linux/module.h>    // Incluido para todos los modulos de kernel
#include <linux/kernel.h>    // Incluido para KERN_INFO
#include <linux/init.h>        // Incluido para __init y __exit macros

static int __init testmodulo_init(void)
{
   printk(KERN_INFO "testmodulo cargado\n"); //Se loga en el log del sistema
   //Aqui iria el codigo a ejecutar
   return 0;    // Si el retorno no es 0 
                //quiere decir que el modulo no se ha podido cargar
}

static void __exit testmodulo_cleanup(void)
{
   printk(KERN_INFO "Informacion de cleanup\n"); //se loga en el /var/log/messages
}
module_init(testmodulo_init);
module_exit(testmodulo_cleanup);
