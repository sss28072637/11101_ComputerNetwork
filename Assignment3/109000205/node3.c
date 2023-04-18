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
} dt3;


extern void read3(FILE *file)
{
  //  Initialize distance_table to 999
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      dt3.costs[i][j] = 999;
    }
  }

  char *line = NULL;
  size_t len = 0;

  // Read file
  getline(&line, &len, file);
  sscanf(line, "%d %d %d %d", &dt3.costs[0][0], &dt3.costs[1][1], &dt3.costs[2][2], &dt3.costs[3][3]);

}

extern void rtinit3()
{
  /* TODO */
  int id = 3;
  struct rtpkt packet;

  for(int i = 0; i < 4; i++) {
    packet.mincost[i] = 999;
  }

  for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if (dt3.costs[i][j] < packet.mincost[i]) {
        packet.mincost[i] = dt3.costs[i][j];
			}
		}
	}

  // only send packet to node 0 and node 2
  for(int i = 0; i < 4; i++) {
    if(i != 1 && i != 3) {
      packet.sourceid = id;
      packet.destid = i;
      tolayer2(packet);
    }
  }
}

extern void rtupdate3(struct rtpkt *rcvdpkt)
{
  /* TODO */
  int changed = 0;  // check update or not
  int tempCost[4];  // store the min cost of rcvpkt so far
  int des = rcvdpkt->destid;
  int src = rcvdpkt->sourceid;

  for(int i = 0; i < 4; i++) {
    tempCost[i] = rcvdpkt->mincost[i];
  }

  int min = 999;
  int old_shortest[4] = {999, 999, 999, 999};
  old_shortest[3] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt3.costs[%d][%d] = %d\n", i, j, dt3.costs[i][j]);
      if(dt3.costs[i][j] < min && dt3.costs[i][j] >= 0)
        min = dt3.costs[i][j];

    }
    old_shortest[i] = min;
    min = 999;
  }

  for(int i = 0; i < 4; i++) {
    int curCost = tempCost[i] + dt3.costs[src][src];
    dt3.costs[i][src] = curCost;
  }

  min = 999;
  int shortest[4] = {999, 999, 999, 999};
  shortest[3] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt3.costs[%d][%d] = %d\n", i, j, dt3.costs[i][j]);
      if(dt3.costs[i][j] < min && dt3.costs[i][j] >= 0)
        min = dt3.costs[i][j];

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
        if(dt3.costs[i][j] < updatePkt.mincost[i])
          updatePkt.mincost[i] = dt3.costs[i][j];
      }
    }

    // only send packet to node 0 and node 2
    for(int i = 0; i < 4; i++) {
      if(i != 1 && i != 3) {
        updatePkt.sourceid = 3;
        updatePkt.destid = i;
        tolayer2(updatePkt);
      }
    }
  }
  
}

void printdt3(void) 
{
  struct distance_table *dtptr = &dt3;

  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}

extern void print_min_cost3()
{

  int min_cost[4];

  for (int i = 0; i < 4; i++)
  {
    int min = dt3.costs[i][i];
    for (int j = 0; j < 4; j++)
    {
      min = min > dt3.costs[i][j] ? dt3.costs[i][j] : min;
    }

    min_cost[i] = min;
  }

  printf("Min cost %d : %d %d %d %d\n", 3, min_cost[0], min_cost[1],
         min_cost[2], min_cost[3]);
}
