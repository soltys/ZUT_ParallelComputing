#include <iostream>
#include "ppm.h"
#include "FiltersMasks.h"
#include <omp.h>


int getNorm(int tab[], int size)
{
    int ret = 0;
    for(int i = 0;i<size;i++)
        for(int j = 0; j < size; j++)
            ret += tab[i+size*j];
    return ret == 0? 1: ret;
}
PPMImage* LinearFilter(PPMImage *img, int filter[], int filterSize) {
    int i, j, rsume, gsume, bsume;
    int margin = (filterSize - 1) / 2;
    int Norm = getNorm(filter, filterSize); 
    
    PPMImage* retImg = new PPMImage;
    retImg->x = img->x;
    retImg->y = img->y;
    retImg->data = new PPMPixel[img->x * img->y];
    if(img){
        double start = omp_get_wtime(); 
//        omp_set_num_threads(2);
        #pragma omp parallel for shared(img, retImg, margin, filter, filterSize, Norm) private(i, j, rsume, bsume, gsume)
        for(i = margin; i < img->x - margin; i++) {
            int sum = 0;
            for(j = margin; j < img->y - margin; j++) {

                rsume = 0;
                gsume = 0;
                bsume = 0;
                for (int k=0; k < filterSize; k++) {
                    for (int l=0; l<filterSize; l++)
                    {                                     
                        rsume += filter[k * filterSize + l] * img->data[(i * img->x) + k + (j % img->y) + l].red;
                        gsume += filter[k * filterSize + l] * img->data[(i * img->x) + k + (j % img->y) + l].green;
                        bsume += filter[k * filterSize + l] * img->data[(i * img->x) + k + (j % img->y) + l].blue;
                    }
                }
                rsume /= Norm;
                gsume /= Norm;
                bsume /= Norm;
                 
                if (rsume > 255) rsume = 255;
                else if (rsume < 0) rsume = 0;

                if (gsume > 255) gsume = 255;
                else if (gsume < 0) gsume = 0;

                if (bsume > 255) bsume = 255;
                else if (bsume < 0) bsume = 0;

                retImg->data[(i * img->x) + (j%img->y)].red = rsume;
                retImg->data[(i * img->x) + (j%img->y)].green = gsume;
                retImg->data[(i * img->x) + (j%img->y)].blue = bsume;
            }
        }
        std::cout << omp_get_wtime() - start << std::endl;
    }
    return retImg;
}
int main() {
    PPMImage *image;
    image = readPPM("lena.ppm");

    PPMImage* filtredLaplace = LinearFilter(image, LAPL1, LAPL1_SIZE);
    writePPM("LAPL1.ppm", filtredLaplace);

    PPMImage* w = LinearFilter(image, GAUSS4, GAUSS4_SIZE);
    writePPM("GAUSS4.ppm", w);

    delete image;
    delete filtredLaplace;
    delete w;
}
