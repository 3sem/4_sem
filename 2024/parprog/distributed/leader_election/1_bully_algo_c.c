/*
 * Email: engineer.rc1@gmail.com 
 * @author Tomrock D'souza, St. Francis Institute Of Technology, University of Mumbai, 2017
 * @copyright GNU General Public License v3.0
 * No reproduction in whole or part without maintaining this copyright notice
 * Example: https://s29.postimg.org/wm3rbxhaf/Bully_Algorithm.png
 */
#include < stdio.h >
#include < conio.h >
#include < stdlib.h >
  int random_number(int min_num, int max_num) {
    int result = 0, low_num = 0, hi_num = 0;
    if (min_num < max_num) {
      low_num = min_num;
      hi_num = max_num + 1; // include max_num in output
    } else {
      low_num = max_num + 1; // include max_num in output
      hi_num = min_num;
    }
    srand(time(NULL));
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
  }
void main() {
  int a[10], n, i, x, u, b[10], m = 0;
  printf("Enter The number of nodes");
  scanf("%d", & n);
  for (i = 0; i < n; i++) {
    printf("Enter state of node %d (0=dead/1=alive): ", i + 1);
    scanf("%d", & a[i]);
  }
  printf("List of alive Nodes:");
  for (i = 0; i < n; i++) {
    if (a[i] > 0) {
      b[m] = i;
      printf("%d ", b[m] + 1);
      m++;
    }
  }
  printf("\nEnter the node that didn't get the reply from the above nodes:");
  scanf("%d", & x);
  for (i = 0; i < m; i++) {
    if (x - 1 == b[i]) {
      x = i;
      break;
    }
  }
  while (x != m - 1) {
    printf("Broadcasting Election message from %d to: ", b[x] + 1);
    for (i = b[x] + 2; i < n + 1; i++) {
      printf("%d ", i);
    }
    x = random_number(x + 1, m - 1);
    printf("\nNew node is %d with fastest response\"I'm alive\"\n", b[x] + 1);
    50.
  }
  printf("Broadcasting Election message from %d to: ", b[x] + 1);
  for (i = b[x] + 2; i < n + 1; i++) {
    printf("%d ", i);
  }
  printf("\nNo Message Returned\nNode %d is Declaring itself as the co-ordinator", b[x] + 1);
  getch();
}
