#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <debug.h>
#include <avconv_util.h>

pid_t recording_pid;

#define RECORD_COMMAND


// -vf "drawtext=fontsize=40:fontfile=FreeSerif.ttf:textfile=/path/livetext.txt:x=(w-text_w)/2:y=(h-text_h)/2:reload=1"
void avconv_record() {
  recording_pid = fork();

  if (recording_pid == 0) {
    DEBUG_PRINT("%s\n", "Recording Start!");
    int fd = open("/dev/null", O_WRONLY);
    dup2(fd, 1);
    dup2(fd, 2);
    execlp("sh", "sh", "-c", "avconv -f v4l2 -framerate 30 -i /dev/video0 -video_size 640x480 http://183.109.159.16:9980/webcam.ffm -f fbdev -pix_fmt bgra -vf \"drawtext=fontcolor=white:fontsize=40:fontfile=FreeSerif.ttf:textfile=/home/pi/lidar.txt:x=((w-text_w)/2)+150:y=((h-text_h)/2)+50:reload=1\" /dev/fb0", NULL);
    // 192.168.1.192
		//execvp(RECORD_COMMAND, NULL);
	}
}

void avconv_stop() {
  int status;
  DEBUG_PRINT("%s [%d]\n", "Recording Stop!", recording_pid);

  kill(recording_pid, 2);
  if (kill(recording_pid, 0) != 0) {
    kill(recording_pid, 2);
    waitpid(recording_pid, &status, 0);
    recording_pid = 0;
  }

  system("killall avconv");
  system("killall ffplay");
}
