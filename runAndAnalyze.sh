#/bin/bash!

cp run2.mac run3.mac

echo "/lightGuide/surfaceSigmaAlpha 0.${1}" >> run3.mac
echo "/lightGuide/surfaceProperty SPECULARLOBECONSTANT 0.000002 .${2} 0.000008 0.${2}" >> run3.mac
echo "/lightGuide/surfaceProperty SPECULARSPIKECONSTANT 0.000002 0.${3} 0.000008 0.${3}" >> run3.mac
echo '/lightGuide/surfaceProperty BACKSCATTERCONSTANT 0.000002 .05 0.000008 .05' >> run3.mac
echo '/lightGuide/surfaceProperty REFLECTIVITY 0.000002 .99 0.000008 .99' >> run3.mac
echo '/run/beamOn 1000000 ' >> run3.mac

#./lightGuide -c models/Winston_cone.stl stl -m run3.mac -o Winston_SA0-$1
./lightGuide -m run3.mac -o SA0-$1
./plotHisto SA0-$1

rm run3.mac
