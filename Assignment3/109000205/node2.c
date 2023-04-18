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
} dt2;


extern void read2(FILE *file)
{
  //  Initialize distance_table to 999
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      dt2.costs[i][j] = 999;
    }
  }

  char *line = NULL;
  size_t len = 0;

  // Read file
  getline(&line, &len, file);
  sscanf(line, "%d %d %d %d", &dt2.costs[0][0], &dt2.costs[1][1], &dt2.costs[2][2], &dt2.costs[3][3]);

}

extern void rtinit2()
{
  /* TODO */
  int id = 2;
  struct rtpkt packet;

  for(int i = 0; i < 4; i++) {
    packet.mincost[i] = 999;
  }

  for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if (dt2.costs[i][j] < packet.mincost[i]) {
        packet.mincost[i] = dt2.costs[i][j];
			}
		}
	}

  // send packet to node0, node 1 and node 3
  for(int i = 0; i < 4; i++) {
    if(i != 2) {
      packet.sourceid = id;
      packet.destid = i;
      tolayer2(packet);
    }
  }

}

extern void rtupdate2(struct rtpkt *rcvdpkt)
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
  old_shortest[2] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt2.costs[%d][%d] = %d\n", i, j, dt2.costs[i][j]);
      if(dt2.costs[i][j] < min && dt2.costs[i][j] >= 0)
        min = dt2.costs[i][j];

    }
    old_shortest[i] = min;
    min = 999;
  }

  for(int i = 0; i < 4; i++) {
    int curCost = tempCost[i] + dt2.costs[src][src];
    dt2.costs[i][src] = curCost;
  }

  min = 999;
  int shortest[4] = {999, 999, 999, 999};
  shortest[2] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      // printf("dt2.costs[%d][%d] = %d\n", i, j, dt2.costs[i][j]);
      if(dt2.costs[i][j] < min && dt2.costs[i][j] >= 0)
        min = dt2.costs[i][j];

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
        if(dt2.costs[i][j] < updatePkt.mincost[i])
          updatePkt.mincost[i] = dt2.costs[i][j];
      }
    }

    // only send packet to node 0 and node 2
    for(int i = 0; i < 4; i++) {
      if(i != 2) {
        updatePkt.sourceid = 2;
        updatePkt.destid = i;
        tolayer2(updatePkt);
      }
    }
  }

}

void printdt2(void) 
{
  struct distance_table *dtptr = &dt2;

  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}

extern void print_min_cost2()
{

  int min_cost[4];

  for (int i = 0; i < 4; i++)
  {
    int min = dt2.costs[i][i];
    for (int j = 0; j < 4; j++)
    {
      min = min > dt2.costs[i][j] ? dt2.costs[i][j] : min;
    }

    min_cost[i] = min;
  }

  printf("Min cost %d : %d %d %d %d\n", 2, min_cost[0], min_cost[1],
         min_cost[2], min_cost[3]);
}
