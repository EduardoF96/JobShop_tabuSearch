//Autor Eduardo Flores Gomez



	#include <stdio.h>  
	#include <stdlib.h>
	#include<time.h>
	#include <string.h>
	#define nMaq 10
	#define nJob 20
	#define MaxIte 100
	struct struct_job{
		int job;
		int nOpe;
		int maquina;
		int timeOp;
	};
	
	struct job_sol{
		int nJ; //número de trabajo
		int nO; //número de operación
		int m;  //número de maquina
		int tO; //tiempo de operacion
		int tS; //tiempo de inicio
		int tE; //tiempo de final
	};
	
	int na; /*para generar números aleatorios*/
	
	
	int numJobs;
	int numMach;
	int i,j;
	int ma,to;
	
	
	/*solo sirve para lectura y generar la solución inicial*/
	struct_job maIni[nJob][nMaq];
	/*sirve para crear la calendarización de la maquina*/
	int aMaqT[nMaq][3000];
	/*sirve para ubicar en que tiempo se quedo el trabajo*/
	int aJobT[nJob];
	/*sirve para generar la solución inicial*/
	int jobOpe[nJob];
	
	
	
	job_sol solInicial[nMaq*nJob];
	
	job_sol solGlobal[nMaq*nJob];
	
	job_sol solActual[nMaq*nJob];
	
	job_sol solVecina[nMaq*nJob];
	
	job_sol mejorLocal[nMaq*nJob];
	
	job_sol mejorNoTabu[nMaq*nJob];
	
	
	int mejorMS=0; //makespam de la solucion globa
	int actualMS=0; //makeSpan de la actual 
	
	/*sirve para almacenar la lista tabu*/
	int listaTabu[nMaq*nJob];
	int tamTabu=((nMaq+nJob)/2)+1;
	
	int tabu1Glo;
	int tabu2Glo;
	
	int tabu1Loc;
	int tabu2Loc;
	
	/*para almacenar los candidatos a realizar el swap*/
	int canSwa[nMaq*nJob];
	int contFact;
	
	int tipoLocal;
	int tipoNoTabu;
	
	
	
	
	void lectura()
	{
		FILE *fichero;
		int no=0;
	    char nombre[100] = "la30.in";
	    fichero = fopen( nombre, "r" );
	    if( fichero )
	    {
	    	fscanf(fichero,"%d",&numJobs);
	    	
	    	fscanf(fichero,"%d",&numMach);
	    	for(j=0;j<numJobs;j++)
	    	{
	    		for(i=0;i<numMach;i++)
	    	    {
	    		fscanf(fichero,"%d%d",&ma,&to);
	    		maIni[j][i].maquina=ma;
	    		maIni[j][i].timeOp=to;
	    		maIni[j][i].nOpe=no;
	    		maIni[j][i].job=j;
	    		no++;//numero de operacion
		     	}	
			}
		}else
		{
			printf("ocurrio un problema \n");
		}
		
	}
	
	
	/**generar solución inicial*/
	void solIni()
	{
		int numT=nMaq*nJob;
		int cont=0;
		int na;
		while (numT>0)
		{
			na = rand() % (nJob);
			
			if(jobOpe[na]<nMaq)
			{
			 printf(" %d ",maIni[na][jobOpe[na]].nOpe);	
		     solInicial[cont].nO=maIni[na][jobOpe[na]].nOpe;
		     solInicial[cont].m=maIni[na][jobOpe[na]].maquina;
		     solInicial[cont].tO=maIni[na][jobOpe[na]].timeOp;
		     solInicial[cont].nJ=maIni[na][jobOpe[na]].job;
			 numT--;
			 jobOpe[na]=jobOpe[na]+1;
			 cont++;
			}	
			
		}
		printf("\n");
	}
	void imSolActual(struct job_sol sol[nMaq*nJob])
	{
		int h;
		for(h=0;h<nMaq*nJob;h++)
		{
			printf(" %d ",sol[h].nO);
		}
		printf("\n");
	}
	void imprimirMatriz()
	{
	    	for(j=0;j<nJob;j++)
	    	{
	    		for(i=0;i<nMaq;i++)
	    	    {	
	    	    printf("%d ",maIni[j][i].nOpe);
	    	    printf("%d ",maIni[j][i].maquina);
	    	    printf("%d ",maIni[j][i].timeOp);
	    	    printf(" ");
		     	}	
		     	printf("\n");
			}	
	}
	void inicializarMat()
	{
		for(i=0;i<nMaq;i++)
		{
			for(j=0;j<3000;j++)
		    {
			aMaqT[i][j]=-1;
	    	}
		}
		
		for(i=0;i<nJob;i++)
		{
			aJobT[i]=0;
		}
	}
	
	int crcaMakeSpan(struct job_sol sol[nMaq*nJob])
	{
		int mayor=0;
		int m; //representa la maquina
		inicializarMat();
		int conEsp=0;
		int k;
		for(i=0;i<nMaq*nJob;i++)
		{
			m=sol[i].m;
			/*primer asignación*/
			if(i==0)
			{
				sol[i].tS=0;
				sol[i].tE=sol[i].tS+sol[i].tO;
				for(j=0;j<sol[i].tO;j++)
				{
					aMaqT[m][j]=sol[i].nO;
				}
				aJobT[sol[i].nJ]=sol[i].tE;
			}else
			{
				conEsp=0;
				for(j=aJobT[sol[i].nJ];j<3000;j++)
				{
					if(aMaqT[m][j]==-1)
					{
						conEsp=conEsp+1;
						if(conEsp==sol[i].tO)
						{
						 for(k=j;k>j-conEsp;k--)
						  {
						 	aMaqT[m][k]=sol[i].nO;
						 	
						  }
						  sol[i].tS=k+1;
			              sol[i].tE=j+1;
			              aJobT[sol[i].nJ]=sol[i].tE;
						break;  	
						}
					}else
					{
					conEsp=0;	
					}
				
				}
			}
			
		}
		
		i=0;
		mayor=sol[i].tE;
		for(i=1;i<nMaq*nJob;i++)
		{
			if(mayor<sol[i].tE)
			{
				mayor=sol[i].tE;
			}
		}
		printf(" %d \n",mayor);
		return mayor;
	}
	
	void imprimirFormato(struct job_sol solucion[nMaq*nJob])
	{
		FILE *out;
		int make;
		char snum[5];
		char salida[100] = "la30";
		make=crcaMakeSpan(solGlobal);
		itoa(make, snum, 10);
		strcat(salida,"_");
        strcat(salida,snum);
        strcat(salida,"_");
        strcat(salida,".out");
        
        out=fopen(salida,"wb+");
        if(out!=NULL)
	    {
		    fprintf(out,"Trabajo//Operacion//Tiempo \r\n");
		    int k=0;
	    	for(j=0;j<nJob;j++)
	    	{
	    		for(i=0;i<nJob*nMaq;i++)
	    	    {
	    	    	if(solucion[i].nJ==j)
	    	    	{
	    	    		fprintf(out,"%d %d %d\r\n",solucion[i].nJ,k,solucion[i].tS);
	    	    		k++;
					}
			    }
			    k=0;
			}
			make=crcaMakeSpan(solGlobal);
			//printf("makespan: %d",make);
			fprintf(out,"makespan: %d \n",make);
		}
		fclose(out);		
	}

	
	void iniListaTabu()
	{
		for(i=0;i<nJob*nMaq;i++)
		{
			listaTabu[i]=0;
		}
	}
	
	void decrementarTL()
	{
		int d;
		for(d=0;d<nJob*nMaq;d++)
		{
			if(listaTabu[d]>0)
			{
				listaTabu[d]=listaTabu[d]-1;
			}
		}
	}
	
	void copiar(struct job_sol sol1[nMaq*nJob],struct job_sol sol2[nMaq*nJob])
	{
		int h;
		for(h=0;h<nJob*nMaq;h++)
		{
		sol1[h].m=sol2[h].m;
		sol1[h].nJ=sol2[h].nJ;
		sol1[h].nO=sol2[h].nO;
		sol1[h].tE=sol2[h].tE;
		sol1[h].tO=sol2[h].tO;
		sol1[h].tS=sol2[h].tS;
		}	
	}
	void recorrido(struct job_sol solV[nMaq*nJob], int n1, int n2)
	{
		int h;
		
		int nj;
		int no;
		int m;
		int to;
		int ts;
		int te;
		
		
		int auxM;
		int auxNJ;
		int auxNO;
		int auxTE;
		int auxTO;
		int auxTS;
		
		 m=solV[n2].m;
		 nj=solV[n2].nJ;
		 no=solV[n2].nO;
		 te=solV[n2].tE;
		 to=solV[n2].tO;
		 ts=solV[n2].tS;
		
		solV[n2].m=solV[n1].m;
	    solV[n2].nJ=solV[n1].nJ;
	    solV[n2].nO=solV[n1].nO;
	    solV[n2].tE=solV[n1].tE;
	    solV[n2].tO=solV[n1].tO;
	    solV[n2].tS=solV[n1].tS;
	    
	    
	    if(n2<n1)
	    {
	
	     for(h=n2+1;h<=n1;h++)
	     {
	     	auxNJ=solV[h].nJ;
			auxNO=solV[h].nO;
			auxM=solV[h].m;
			auxTO=solV[h].tO;
			
			solV[h].nJ=nj;
		    solV[h].nO=no;
	    	solV[h].m=m;
	    	solV[h].tO=to;
	    	
			nj=auxNJ;
		    no=auxNO;
		    m=auxM;
		    to=auxTO;
	     }
	    }else
	    {
	    for(h=n2-1;h>=n1;h--)
	     {
	     	auxNJ=solV[h].nJ;
			auxNO=solV[h].nO;
			auxM=solV[h].m;
			auxTO=solV[h].tO;
			
			solV[h].nJ=nj;
		    solV[h].nO=no;
	    	solV[h].m=m;
	    	solV[h].tO=to;
	    	
			nj=auxNJ;
		    no=auxNO;
		    m=auxM;
		    to=auxTO;
	     }	
	    }
	}
	void swap(struct job_sol solV[nMaq*nJob], int n1, int n2)
	{
		//printf("cambio: %d %d \n",n1,n2);
		int auxM=solV[n1].m;
		int auxNJ=solV[n1].nJ;
		int auxNO=solV[n1].nO;
		int auxTE=solV[n1].tE;
		int auxTO=solV[n1].tO;
		int auxTS=solV[n1].tS;
			
		solV[n1].m=solV[n2].m;
		solV[n1].nJ=solV[n2].nJ;
		solV[n1].nO=solV[n2].nO;
		solV[n1].tE=solV[n2].tE;
		solV[n1].tO=solV[n2].tO;
		solV[n1].tS=solV[n2].tS;
		
		solV[n2].m=auxM;
		solV[n2].nJ=auxNJ;
		solV[n2].nO=auxNO;
		solV[n2].tE=auxTE;
		solV[n2].tO=auxTO;
		solV[n2].tS=auxTS;
		
	}
	int explVecindario(struct job_sol sol[nMaq*nJob])
	{
		 //variable que sirve para verificar si el movimiento incluye algo tabu
	   int tabu=0;
	   //array para almacenar los candidatos canSwa[];
	   contFact=0; 
	   int bandera=0;
	   int bandera2=0;
	   int tam=nJob*nMaq-1;
	   int na2;
	   int l;
	   int canFal;
	   
	   int localMejorMS=3000;
	   int noTabuMS=30000;
	   int vecMS;
	   /*numero de la cadena de solución a la ue se aplicara el swap*/
	   int z;
	   int k;
	   
	   int tmin;
	   int tmax;
	   
	   int s;
	   //printf("la operacion a calcular: %d \n",solucion[na].nO);
	   for(s=0;s<nMaq*nJob;s++)
	   {
	     contFact=0;
	     //printf("soy la operacion: %d \n",sol[s].nO); 
	     l=0; /*sirve para recorrer las posibilidades */
	     canFal=0;
	     bandera=0;
	     bandera2=0;
	     j=s-1;
	     while(bandera==0)
		 {	
			
			if(j<0)
			{
				bandera=1;
				j=0;
				break;
			}
			if(sol[s].nJ==sol[j].nJ)
			{
				//encontramos el tope
				bandera=1;
				j++;
				
			}else
			{
				for(l=j+1;l<s;l++)
				{
					if(sol[j].nJ==sol[l].nJ)
					{
						canFal=1;
						break;
					}
				}
				if(canFal==0)
				{
					canSwa[contFact]=j;
					contFact++;
				}
				j--;
				canFal=0;
			}
		  };
	 
	    k=s+1; 
		while(bandera2==0)
		{
			if(k>tam)
			{
				bandera2=1;
				k=tam;
				break;
			}
			if(sol[s].nJ==sol[k].nJ)
			{
				//encontramos el tope
				bandera2=1;
				k--;
				
			}else
			{
				for(l=k-1;l>s;l--)
				{
					if(sol[k].nJ==sol[l].nJ)
					{
						canFal=1;
						break;
					}
				}
				if(canFal==0)
				{
					canSwa[contFact]=k;
					contFact++;
				}
				canFal=0;
				k++;
			}
		};
		tmin=j;
		tmax=k;
		if(sol[j].nO==sol[k].nO)
		{
		 printf("no se puede implementar nada\n");
		}else
		{
			for(z=0;z<contFact;z++)
			{
				copiar(solVecina,sol);
				swap(solVecina,s,canSwa[z]);
				vecMS=crcaMakeSpan(solVecina);
				if(vecMS<localMejorMS)
				{
					copiar(mejorLocal,solVecina);
					localMejorMS=vecMS;
					
					
					tabu1Glo=sol[s].nO;
				    tabu2Glo=sol[canSwa[z]].nO;
					tipoLocal=1;
				}
				if(listaTabu[sol[s].nO]==0 && listaTabu[sol[z].nO]==0)
				{
				    if(vecMS<noTabuMS)
				    {
				    //printf("Mejoro no tabu\n");
					copiar(mejorNoTabu,solVecina);
					noTabuMS=vecMS;
					
					
					//movimientos poner tabu
				    tabu1Loc=sol[s].nO;
				    tabu2Loc=sol[canSwa[z]].nO;
				    
				    tipoNoTabu=1;
				    }
				}
			}
			/*aqui va el recorrido*/
		
			for(z=tmin;z<tmax;z++)
			{
				if(z!=s)
				{
				copiar(solVecina,sol);
				recorrido (solVecina,s,z);
				vecMS=crcaMakeSpan(solVecina);
				if(vecMS<localMejorMS)
				{
					printf("Si entro\n000");
					copiar(mejorLocal,solVecina);
					localMejorMS=vecMS;
					tipoLocal=2;
					tabu1Glo=sol[s].nO;
				}
				//preguntamos si es tabu
				if(listaTabu[sol[s].nO]==0 )
				{
				    if(vecMS<noTabuMS)
				    {
				    printf("Si entro\n000");
				    //printf("Mejoro no tabu\n");
					copiar(mejorNoTabu,solVecina);
					noTabuMS=vecMS;
					
					tipoNoTabu=2;
					tabu1Loc=sol[s].nO;
					
					
				    }
				}
				}	
			}
				
		}
	
	   }
	
		return tabu;
	}
	
	int main() 
	{
		
		lectura();
		srand(time(NULL));
		imprimirMatriz();
		printf("\n");
		int b;
		solIni();
		//asignar solucion global con inicial
		copiar(solGlobal,solInicial);
		//asignar solucion actual con global
		copiar(solActual,solInicial);
		//copiar la mejor local con la inicial
		copiar(mejorLocal,solInicial);
		//copiar la mejor no tabu con la inicial
		copiar(mejorNoTabu,solInicial);
		
		iniListaTabu();
		
		int mejorGlobal=crcaMakeSpan(solGlobal);
		
		actualMS=crcaMakeSpan(solActual);
		
		
		int t=0;
		int mejLoc;
		int status;
	
		int iterSinCambio=0;
		do
		{
			status=explVecindario(solActual);
			mejLoc=crcaMakeSpan(mejorLocal);
		    imSolActual(solActual); 
			if(mejLoc<mejorGlobal)
			{
				copiar(solGlobal,mejorLocal);
				copiar(solActual,mejorLocal);
				//poner tabu el movimiento 
				if(tipoLocal==1)
				{
				listaTabu[tabu1Glo]=listaTabu[tabu1Glo]+tamTabu;
			    listaTabu[tabu2Glo]=listaTabu[tabu2Glo]+tamTabu;	
				}else
				{
				listaTabu[tabu1Glo]=listaTabu[tabu1Glo]+tamTabu;
				}
			    iterSinCambio=0;
			    mejorGlobal=mejLoc;
			}else
			{
				//copiar la mejor solucion no tabu
				copiar(solActual,mejorNoTabu);
				if(tipoNoTabu==1)
				{
				listaTabu[tabu1Loc]=listaTabu[tabu1Loc]+tamTabu;
				listaTabu[tabu2Loc]=listaTabu[tabu2Loc]+tamTabu;	
				}else
				{
				listaTabu[tabu1Loc]=listaTabu[tabu1Loc]+tamTabu;
				}
				iterSinCambio++;
			}
		decrementarTL();
		printf("\n");
		for(i=0;i<nMaq*nJob;i++)
		{
			printf(" %d ",listaTabu[i]);
		}
		printf("\n");
		}while(iterSinCambio<MaxIte);
		
	    imSolActual(solGlobal); 
	    mejorGlobal=crcaMakeSpan(solGlobal);
	  	imprimirMatriz();
		imprimirFormato(solGlobal);
	/*	for(i=0;i<nMaq*nJob;i++)
		{
			printf(" %d ",listaTabu[i]);
		}	*/
		return 0;
	}
