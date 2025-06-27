#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/ioctl.h>

#define LED_DEV "/dev/my_led"
#define BTN_DEV "/dev/my_btn"

#define LED_IOCTL_MAGIC  'L'
#define LED_IOCTL_TOGGLE _IO(LED_IOCTL_MAGIC, 0)

static int led_state = 0;

int main() {
   int led_fd, btn_fd;
   char cmd;
   char btn_state;

    // Mở device file
   led_fd = open(LED_DEV, O_WRONLY);
   if (led_fd < 0) {
      perror("Failed to open LED device");
      return 1;
   }

   btn_fd = open(BTN_DEV, O_RDONLY);
   if (btn_fd < 0) {
      perror("Failed to open Button device");
      close(led_fd);
      return 1;
   }

   while (1) {
      printf("\nEnter command:\n");
      printf("1 - Turn ON LED\n");
      printf("0 - Turn OFF LED\n");
      printf("r - Read button status\n");
      printf("q - Quit\n");
      printf("t - Toggle LED (ioctl)\n");
      printf("m - Monitor button and toggle LED automatically\n");
      printf(">> ");
      cmd = getchar();
      while (getchar() != '\n'); // Clear buffer

      if (cmd == '1' || cmd == '0') {
         write(led_fd, &cmd, 1);
      } else if (cmd == 'r') {
         if (read(btn_fd, &btn_state, 1) == 1) {
            printf("Button state: %c\n", btn_state);  // '1' hoặc '0'
         } else {
            perror("Failed to read button");
         }
      } else if (cmd == 'q') {
         break;
      } else if (cmd == 't') {
         if (ioctl(led_fd, LED_IOCTL_TOGGLE) < 0)
         perror("ioctl failed");
      }
      else if (cmd == 'm') {
         char last_state = '0';
         printf("Monitoring button... Press 'Ctrl+C' to stop.\n");
         while (1) {
            if (read(btn_fd, &btn_state, 1) == 1) {
               if (btn_state == '1' && last_state == '0') {
           
                  if (ioctl(led_fd, LED_IOCTL_TOGGLE) < 0)
                    perror("ioctl failed");
               }
            last_state = btn_state;
            }
            usleep(100000); // 100ms
         }
      }else {
            printf("Unknown command!\n");
      }
   }

   close(led_fd);
   close(btn_fd);
   return 0;
}