#include <stdio.h>
#include <omp.h>
#include <math.h>
 int main()
 {
omp_set_nested(1);
printf("is nested :%d \n",	omp_get_nested());


          /* screen ( integer) coordinate */
        int iX,iY;
	unsigned char tablica[800][800][3];
        const int iXmax = 800; 
        const int iYmax = 800;
        /* world ( double) coordinate = parameter plane*/
        double Cx,Cy;
        const double CxMin=-2.5;
        const double CxMax=1.5;
        const double CyMin=-2.0;
        const double CyMax=2.0;
        /* */
        double PixelWidth=(CxMax-CxMin)/iXmax;
        double PixelHeight=(CyMax-CyMin)/iYmax;
        /* color component ( R or G or B) is coded from 0 to 255 */
        /* it is 24 bit color RGB file */
        const int MaxColorComponentValue=255; 
        FILE * fp;
        FILE * blurp;
        char *filename="new1.ppm";
        char *blurname="blur.ppm";
        char *comment="# ";/* comment should start with # */
        static unsigned char color[3];
        /* Z=Zx+Zy*i  ;   Z0 = 0 */
        double Zx, Zy;
        double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
        /*  */
        int Iteration;
        const int IterationMax=200;
        /* bail-out value , radius of circle ;  */
        const double EscapeRadius=2;
        double ER2=EscapeRadius*EscapeRadius;
        /*create new file,give it a name and open it in binary mode  */
        fp= fopen(filename,"wb"); /* b -  binary mode */
        blurp= fopen(blurname,"wb"); /* b -  binary mode */
        /*write ASCII header to the file*/
        fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,iXmax,iYmax,MaxColorComponentValue);
        fprintf(blurp,"P6\n %s\n %d\n %d\n %d\n",comment,iXmax,iYmax,MaxColorComponentValue);
        /* compute and write image data bytes to the file*/
	#pragma omp parallel for schedule(dynamic,10) firstprivate(Zx, Zy, Zx2, Zy2, Cx, Cy, Iteration, iX) shared(tablica, PixelHeight, PixelWidth, ER2) 
        for(iY=0;iY<iYmax;iY++)
        {
	
             Cy=CyMin + iY*PixelHeight;
             if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
	
             for(iX=0;iX<iXmax;iX++)
             {         
                        Cx=CxMin + iX*PixelWidth;
                        /* initial value of orbit = critical point Z= 0 */
                        Zx=0.0;
                        Zy=0.0;
                        Zx2=Zx*Zx;
                        Zy2=Zy*Zy;
                        /* */
                        for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
                        {
                            Zy=2*Zx*Zy + Cy;
                            Zx=Zx2-Zy2 +Cx;
                            Zx2=Zx*Zx;
                            Zy2=Zy*Zy;
                        };
                        /* compute  pixel color (24 bit = 3 bytes) */
			
                        if (Iteration==IterationMax)
                        { /*  interior of Mandelbrot set = black */
                           tablica[iY][iX][0] = 0;// (120 * omp_get_thread_num()) % 255;
                           tablica[iY][iX][1] = 0;//(210 * omp_get_thread_num()) % 255;
                           tablica[iY][iX][2] = 0;//(100 * omp_get_thread_num()) % 255;
           		   
                        }
                     	else 
                        { /* exterior of Mandelbrot set = white */
				//printf("%d\n", omp_get_thread_num());
                             tablica[iY][iX][0]= 255;//(50 * omp_get_thread_num()) % 255; /* Red*/
                             tablica[iY][iX][1]= 255;// (80 * omp_get_thread_num()) % 255;  /* Green */ 
                             tablica[iY][iX][2]= 255;//(10 * omp_get_thread_num()) % 255;/* Blue */
                        };
                }
        }

unsigned char t2[800][800][3];
unsigned char* tmp[9];
int i, j;
//#pragma omp parallel sections
{
	//#pragma omp section
        {
            for (i = 0; i < 800; i++)
        	{
		        for (j = 0; j < 800; j++)
		        {
		           tmp[4]  = tablica[i][j];		           
		           
		           if(i-1<0 && j-1 < 0)
		           {
		            tmp[0] = NULL;
		           }
		           else
		           {
		            tmp[0] = tablica[i-1][j-1];
		           }
		           
		            if(i-1<0)
		           {
		            tmp[1] = NULL;
		           }
		           else
		           {
		            tmp[1] = tablica[i-1][j];
		           }

		           if(i-1<0 && j+1 > 800)
		           {
		            tmp[2] = NULL;
		           }
		           else
		           {
		            tmp[2] = tablica[i-1][j+1];
		           }

		           if(j-1 < 0)
		           {
		            tmp[3] = NULL;
		           }
		           else
		           {
		            tmp[3] = tablica[i][j-1];
		           }

		           if( j+1 >800)
		           {
		            tmp[5] = NULL;
		           }
		           else
		           {
		            tmp[5] = tablica[i][j+1];
		           }

		           if(i+1>800 && j-1 < 0)
		           {
		            tmp[6] = NULL;
		           }
		           else
		           {
		            tmp[6] = tablica[i+1][j-1];
		           }

		           if(i+1>800)
		           {
		            tmp[7] = NULL;
		           }
		           else
		           {
		            tmp[7] = tablica[i+1][j];
		           }

		           if(i+1>800 && j+1 >800)
		           {
		            tmp[8] = NULL;
		           }
		           else
		           {
		            tmp[8] = tablica[i+1][j+1];
		           }           
		           

		           int b;
		           int red = 0;
                    int blue = 0;
                    int green = 0;
		           for(b = 0 ; b < 9 ;b++)
		           {
		            if(b == 4) continue;
		            if(tmp[b]==NULL) continue; 
                    red += tmp[b][0];
                    blue += tmp[b][1];
                    green += tmp[b][2];
		           }
		           
		           t2[i][j][0] = red;
                    t2[i][j][1] = blue;
                    t2[i][j][2] = green;    
		           
	            }
            }
           
        }

   //     #pragma omp section
        {
        
        }
}


	/*write color to the file*/
	
	for (i = 0; i < 800; i++)
	{
		for (j = 0; j < 800; j++)
		{
        		fwrite(tablica[i][j],1,3,fp);
        		fwrite(t2[i][j],1,3,blurp);
		}
	}
        fclose(fp);
        fclose(blurp);        
        return 0;
 }