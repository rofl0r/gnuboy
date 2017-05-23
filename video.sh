res="$(xdpyinfo | grep dimensions | awk '{print $2}')"
echo -e "\033[37m\nStarting screencast with a resolution of $res and .ogv as video format \033[0m"
ffmpeg -y -nostats -loglevel 0 -video_size $res -framerate 50 -f x11grab -i :0.0 -f pulse -ac 2 -i default output.mkv -preset ultrafast 2>/dev/null




