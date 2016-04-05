/* invoque las llamadas al sistema donde crea necesario */


/* invoque las llamadas al sistema donde crea necesario */
main(void)
{
	int i;
	char c;
	 i = read(0, &c, 1);

  	while (  i == 1){    
  		write(1, &c, 1);    
    	i = read(0, &c, 1); 

    }
}
