#include <stdio.h>

struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
extern void tolayer2(struct rtpkt packet);

struct distance_table 
{
  int costs[4][4];
} dt1;


extern void read1(FILE *file)
{
  //  Initialize distance_table to 999
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      dt1.costs[i][j] = 999;
    }
  }

  char *line = NULL;
  size_t len = 0;

  // Read file
  getline(&line, &len, file);
  sscanf(line, "%d %d %d %d", &dt1.costs[0][0], &dt1.costs[1][1], &dt1.costs[2][2], &dt1.costs[3][3]);
}

extern void rtinit1() 
{
  /* TODO */
  int id = 1;
  struct rtpkt packet;

  for(int i = 0; i < 4; i++) {
    packet.mincost[i] = 999;
  }

  for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if (dt1.costs[i][j] < packet.mincost[i]) {
        packet.mincost[i] = dt1.costs[i][j];
			}
		}
	}

  // only send packet to node 0 and node 2
  for(int i = 0; i < 4; i++) {
    if(i == 0 || i == 2) {
      packet.sourceid = id;
      packet.destid = i;
      tolayer2(packet);
    }
  }

}

extern void rtupdate1(struct rtpkt *rcvdpkt)
{
  /* TODO */
  int changed = 0;  // check update or not
  int tempCost[4];  // store the min cost of rcvpkt so far
  int des = rcvdpkt->destid;
  int src = rcvdpkt->sourceid;

  if(des != 1)
    printf("error\n");

  for(int i = 0; i < 4; i++) {
    tempCost[i] = rcvdpkt->mincost[i];
  }

  int min = 999;
  int old_shortest[4] = {999, 999, 999, 999};
  old_shortest[1] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt1.costs[%d][%d] = %d\n", i, j, dt1.costs[i][j]);
      if(dt1.costs[i][j] < min && dt1.costs[i][j] >= 0)
        min = dt1.costs[i][j];

    }
    old_shortest[i] = min;
    min = 999;
  }

  for(int i = 0; i < 4; i++) {
    int curCost = tempCost[i] + dt1.costs[src][src];
    dt1.costs[i][src] = curCost;
  }

  min = 999;
  int shortest[4] = {999, 999, 999, 999};
  shortest[1] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt1.costs[%d][%d] = %d\n", i, j, dt1.costs[i][j]);
      if(dt1.costs[i][j] < min && dt1.costs[i][j] >= 0)
        min = dt1.costs[i][j];

    }
    shortest[i] = min;
    min = 999;
  }

  for(int i = 0; i < 4; i++) {
    if(old_shortest[i] != shortest[i])
      changed = 1;
  }

  // prepare a new packet if we need to inform our neighbors
  if(changed == 1) {
    // initialize our update packet
    struct rtpkt updatePkt;
    for(int i = 0; i < 4; i++) {
      updatePkt.mincost[i] = 999;
    }
    // store the mincost to the update packet
    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
        if(dt1.costs[i][j] < updatePkt.mincost[i])
          updatePkt.mincost[i] = dt1.costs[i][j];
      }
    }

    // only send packet to node 0 and node 2
    for(int i = 0; i < 4; i++) {
      if(i != 1 && i != 3) {
        updatePkt.sourceid = 1;
        updatePkt.destid = i;
        tolayer2(updatePkt);
      }
    }
  }

}

void printdt1(void)  
{
  struct distance_table *dtptr = &dt1;

  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}

extern void linkhandler1(int linkid, int newcost)
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
{
  /* TODO */

  int oldcost = dt1.costs[0][0];

  for (int i = 0; i < 4; i++) {
    // printf("%d\n", dt1.costs[i][linkid]);
    int newValue = dt1.costs[i][linkid] - oldcost + newcost;
    // printf("%d = %d - %d + %d\n", newValue, dt1.costs[i][linkid], oldcost, newcost);
    // printf("newvalue in node0: %d\n", newValue);
    dt1.costs[i][linkid] = newValue;
  }

  dt1.costs[0][0] = newcost;

  // printdt1();

  // for(int i = 0; i < 4; i++) {
  //   for(int j = 0; j < 4; j++) {
  //     int curCost = dt1.costs[j][j] + dt1.costs[i][j];
  //     printf("%d = %d + %d\n", curCost, dt1.costs[j][j], dt1.costs[i][j]);
  //     if(curCost < dt1.costs[i][j]) {
  //       dt1.costs[i][j] = curCost;
  //     }
  //   }
  // }

  int min = 999;
  int shortest[4] = {999, 999, 999, 999};
  shortest[1] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt1.costs[%d][%d] = %d\n", i, j, dt1.costs[i][j]);
      if(dt1.costs[i][j] < min && dt1.costs[i][j] >= 0)
        min = dt1.costs[i][j];
      
    }
    shortest[i] = min;
    min = 999;
  }

  // printf("in node1 handler\n");
  // printf("shortest[0]: %d\n", shortest[0]);
  // printf("shortest[1]: %d\n", shortest[1]);
  // printf("shortest[2]: %d\n", shortest[2]);
  // printf("shortest[3]: %d\n", shortest[3]);

  // initialize our update packet
  struct rtpkt updatePkt;
  for(int i = 0; i < 4; i++) {
    updatePkt.mincost[i] = shortest[i];
  }

  //send the update packet to our neighbors
  for(int i = 0; i < 4; i++) {
    if(i != 1 && i != 3) {
      updatePkt.sourceid = 1;
      updatePkt.destid = i;
      tolayer2(updatePkt);
    }
  }

}

extern void print_min_cost1()
{

  int min_cost[4];

  for (int i = 0; i < 4; i++)
  {
    int min = dt1.costs[i][i];
    for (int j = 0; j < 4; j++)
    {
      min = min > dt1.costs[i][j] ? dt1.costs[i][j] : min;
    }

    min_cost[i] = min;
  }

  printf("Min cost %d : %d %d %d %d\n", 1, min_cost[0], min_cost[1],
         min_cost[2], min_cost[3]);
}