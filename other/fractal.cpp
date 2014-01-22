#include <stdio.h>
#include <math.h>
#include <iostream>
#include <omp.h>

 int main()
 {
          /* screen ( integer) coordinate */
        int iX,iY;
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
        char filename[]="new1.ppm";
        char comment[]="# ";/* comment should start with # */
        static unsigned char color[3];

        unsigned char data[iYmax][iXmax][3];
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
        /*write ASCII header to the file*/
        fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,iXmax,iYmax,MaxColorComponentValue);
        /* compute and write image data bytes to the file*/
        double start = omp_get_wtime(); 
        std::cout << omp_get_num_threads();
        #pragma omp parallel for shared(PixelHeight, PixelWidth, data) private(iY, iX, Cy, Cx, Zx, Zy, Zx2, Zy2, color, Iteration)
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
                             data[iY][iX][0]=omp_get_thread_num()*20%255; /* Red*/
                             data[iY][iX][1]=omp_get_thread_num()*40%255;  /* Green */ 
                             data[iY][iX][2]=omp_get_thread_num()*80%255;/* Blue */
                    
                        }
                     else 
                        { /* exterior of Mandelbrot set = white */
                           data[iY][iX][0]=255; /* Red*/
                             data[iY][iX][1]=255 - omp_get_thread_num()*40%255;  /* Green */ 
                             data[iY][iX][2]=255;
                        };
                }
        }
        std::cout << omp_get_wtime() - start << std::endl;
        for(iY=0;iY<iYmax;iY++)
            for(iX=0;iX<iXmax;iX++)
                fwrite(data[iY][iX],1,3,fp);
        fclose(fp);
        return 0;
 }
