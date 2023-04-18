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
  int costs[4][4] ;
} dt0;


extern void read0(FILE *file)
{

  //  Initialize distance_table to 999
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      dt0.costs[i][j] = 999;
    }
  }
  char *line = NULL;
  size_t len = 0;

  // Read file
  getline(&line, &len, file);
  sscanf(line, "%d %d %d %d", &dt0.costs[0][0], &dt0.costs[1][1], &dt0.costs[2][2], &dt0.costs[3][3]);
}


extern void rtinit0() 
{
  /* TODO */

  // count how many outgoing edge
  // int numEdges = 0;
  // for(int i = 0; i < 4; i++) {
  //   for(int j = 0; j < 4; j++) {
  //     if(i == j && i != 0 && dt0.costs[i][j] != 999)
  //       numEdges++;
  //   }
  // }

  int id = 0;
  struct rtpkt packet;

  for(int i = 0; i < 4; i++) {
    packet.mincost[i] = 999;
  }

  for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if (dt0.costs[i][j] < packet.mincost[i]) {
        packet.mincost[i] = dt0.costs[i][j];
			}
		}
	}

  for(int i = 1; i < 4; i++) {
    packet.sourceid = id;
    packet.destid = i;
    tolayer2(packet);
  }
}


extern void rtupdate0(struct rtpkt *rcvdpkt)
{
  /* TODO */

  int changed = 0;  // check update or not
  int tempCost[4];  // store the min cost of rcvpkt so far
  int des = rcvdpkt->destid;
  int src = rcvdpkt->sourceid;

  if(des != 0)
    printf("error\n");

  for(int i = 0; i < 4; i++) {
    tempCost[i] = rcvdpkt->mincost[i];
  }

  int min = 999;
  int old_shortest[4] = {999, 999, 999, 999};
  old_shortest[0] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt0.costs[%d][%d] = %d\n", i, j, dt0.costs[i][j]);
      if(dt0.costs[i][j] < min && dt0.costs[i][j] >= 0)
        min = dt0.costs[i][j];

    }
    old_shortest[i] = min;
    min = 999;
  }

  for(int i = 0; i < 4; i++) {
    int curCost = tempCost[i] + dt0.costs[src][src];
    dt0.costs[i][src] = curCost;
  }

  min = 999;
  int shortest[4] = {999, 999, 999, 999};
  shortest[0] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt0.costs[%d][%d] = %d\n", i, j, dt0.costs[i][j]);
      if(dt0.costs[i][j] < min && dt0.costs[i][j] >= 0)
        min = dt0.costs[i][j];

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
        if(dt0.costs[i][j] < updatePkt.mincost[i])
          updatePkt.mincost[i] = dt0.costs[i][j];
      }
    }

    //send the update packet to our neighbors
    for(int i = 1; i < 4; i++) {
      updatePkt.sourceid = 0;
      updatePkt.destid = i;
      tolayer2(updatePkt);
    }
  }

}


 void printdt0(void) 
{
  struct distance_table *dtptr = &dt0;
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n", dtptr->costs[1][1],
           dtptr->costs[1][2], dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n", dtptr->costs[2][1],
           dtptr->costs[2][2], dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n", dtptr->costs[3][1],
           dtptr->costs[3][2], dtptr->costs[3][3]);
}

extern void linkhandler0(int linkid, int newcost)
/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
{
  /* TODO */
  // printdt0();
  int oldcost = dt0.costs[1][1];

  for (int i = 0; i < 4; i++) {
    // printf("%d\n", dt0.costs[i][linkid]);
    int newValue = dt0.costs[i][linkid] - oldcost + newcost;
    // printf("%d = %d - %d + %d\n", newValue, dt0.costs[i][linkid], oldcost, newcost);
    // printf("newvalue in node0: %d\n", newValue);
    dt0.costs[i][linkid] = newValue;
  }

  dt0.costs[1][1] = newcost;

  // printdt0();

  // for(int i = 0; i < 4; i++) {
  //   for(int j = 0; j < 4; j++) {
  //     int curCost = dt0.costs[j][j] + dt0.costs[i][j];
  //     if(curCost < dt0.costs[i][j]) {
  //       dt0.costs[i][j] = curCost;
  //     }
  //   }
  // }


  // construct a shortest array
  int min = 999;
  int shortest[4] = {999, 999, 999, 999};
  shortest[0] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt0.costs[%d][%d] = %d\n", i, j, dt0.costs[i][j]);
      if(dt0.costs[i][j] < min && dt0.costs[i][j] >= 0)
        min = dt0.costs[i][j];

    }
    shortest[i] = min;
    min = 999;
  }


  // printf("in node0 handler\n");
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
  for(int i = 1; i < 4; i++) {
    updatePkt.sourceid = 0;
    updatePkt.destid = i;
    tolayer2(updatePkt);
  }

}

extern void print_min_cost0()
{
  int min_cost[4];

  for(int i = 0;i < 4;i++)
  {
    int min = dt0.costs[i][i];
    for(int j = 0;j < 4; j++)
    {
      min = min > dt0.costs[i][j] ? dt0.costs[i][j] : min;
    }

    min_cost[i] = min;
  }

  printf("Min cost %d : %d %d %d %d\n", 0, min_cost[0], min_cost[1], min_cost[2], min_cost[3]);
}

