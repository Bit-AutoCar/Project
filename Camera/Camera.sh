export STREAMER_PATH=$HOME/workspace/Project/Camera/mjpg-streamer
export LD_LIBRARY_PATH=$STREAMER_PATH
$STREAMER_PATH/mjpg_streamer -i "input_uvc.so -f 20 -r 200x120 -d /dev/video0 " -o "output_http.so -w $STREAMER_PATH/www"
