export STREAMER_PATH=$HOME/Project/Camera/mjpg-streamer/mjpg-streamer-experimental
export LD_LIBRARY_PATH=$STREAMER_PATH
#$STREAMER_PATH/mjpg_streamer -i "input_raspicam.so -d 100 -r 320x240" -o "output_http.so -w $STREAMER_PATH/www"
#$STREAMER_PATH/mjpg_streamer -i "input_raspicam.so -fps 20 -x 320 -y 240 -ISO 900" -o "output_http.so -w $STREAMER_PATH/www"

#$STREAMER_PATH/mjpg_streamer -i "input_file.so -f 25 /srv/http/" -o "output_http.so -p 8080 -w /srv/"
$STREAMER_PATH/mjpg_streamer -i "input_raspicam.so -f 25 -d /srv/" -o "output_http.so -p 8081 -w /srv/"
