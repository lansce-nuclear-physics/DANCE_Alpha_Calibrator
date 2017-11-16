#!/bin/sh

#for i in `seq 64695 64833`;  #63Cu
for i in `seq 67339 67401`;  #59Co
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
