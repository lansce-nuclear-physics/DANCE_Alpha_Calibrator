#!/bin/sh

#for i in `seq 60622 60811`;  #Pb208
#for i in `seq 62695 62735`;  #Pb208
#for i in `seq 63650 64136`;  #Pb208
#for i in `seq 64834 64987`;  #Pb208
#for i in `seq 68040 68103`;  #Pb208

#for i in `seq 60409 60451`;  #Au197
#for i in `seq 60572 60584`;  #Au197
#for i in `seq 63590 63649`;  #Au197
#for i in `seq 68104 68126`;  #Au197

#for i in `seq 60834 61251`;  #Cu65
#for i in `seq 63015 63589`;  #Cu65
#for i in `seq 64137 64692`;  #Cu65
#for i in `seq 64989 65265`;  #Cu65

for i in `seq 104759 104763`;  #Cu63


#for i in `seq 60828 60833`;  #Y88
#for i in `seq 60364 60369`;  #Y88
#for i in `seq 61292 61296`;  #Y88

do
    echo Processing Run: $i;
    ./DANCE_Alpha_Calibrator $i
done

#./DANCE_Alpha_Calibrator 31929 31930 31931 31932 31933 31934 
#./DANCE_Alpha_Calibrator 31935 31936 31937 31938 31939 
#./DANCE_Alpha_Calibrator 31940 31941 31942 31943 31944
#./DANCE_Alpha_Calibrator 31945 31946 31947 31948 31949
#./DANCE_Alpha_Calibrator 31950 31951 31952 31953 31954 
#./DANCE_Alpha_Calibrator 31955 31956 31957 31958 31959 
#./DANCE_Alpha_Calibrator 31960 31961 31962 31963 31964 
#./DANCE_Alpha_Calibrator 31965 31966 31967 31968 31969 
#./DANCE_Alpha_Calibrator 31970 31971 31972 31973 31974 31975 31976 31977
