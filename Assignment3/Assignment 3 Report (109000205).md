# Assignment 3 Report (109000205)

## Part 1
### Execution Result
![](https://i.imgur.com/o3jzOJy.png)

### Implementation
- First, I copied the `table[4][4]` to an new array called `cost[4][4]`, preventing from modify the `table[4][4]`.
```{.c .numberLines}
int cost[4][4]={infinity};

for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
        if(table[i][j] == 0) {
            cost[i][j] = infinity;
        } else {
            cost[i][j] = table[i][j];
        }
    }
}
```

- Second, update the status of the source node.
```{.c .numberLines}
for(int i = 0; i < 4; i++) {
    nodes[id].dist[i] = cost[id][i];
}

for(int i = 0; i < 4; i++) {
    nodes[id].visit[i] = 0;
}

nodes[id].dist[id] = 0;
nodes[id].visit[id] = 1;
```

- Third, find out the closest node to the source node.
- Then visit the closest node and update the distance to each node via the closest node.
```{.c .numberLines}
int count = 1;
int shortestDistance;
int closestNode;

for(count = 1; count < 3; count++) {

    shortestDistance = infinity;

    int i = 0;
    while (i < 4) {
        if(nodes[id].dist[i] < shortestDistance && nodes[id].visit[i] == 0) {
            shortestDistance = nodes[id].dist[i];
            closestNode = i;
        }
        i++;
    }

    nodes[id].visit[closestNode] = 1;
    i = 0;

    while (i < 4) {
        if(nodes[id].visit[i] == 0) {
            if(shortestDistance + cost[closestNode][i] < nodes[id].dist[i]) {
                nodes[id].dist[i] = shortestDistance + cost[closestNode][i];
            }
        }
        i++;
    }
}
```

### Problems
- I put lots of effort to understanding the procedure of Dilkstra's algorithm and try to convert the concept of the algorithm to the real code.
- I referenced the procedure of Dilkstra's algorithm from the Internet and spent some time on troubleshooting and finally fully realize the code and the steps.
- Reference:
    - https://www.programiz.com/dsa/dijkstra-algorithm

## Part 2
### Execution Result
![](https://i.imgur.com/0AyeedJ.png)

### Implementation
​	Since the function between each node is similar, I will only explain the node0. **The only difference is that node1 and node3 will not communicate with each other since there aren't any directed edge between them, which indicated that they don't need to send the update packet to each other.**
#### rtinit0()
- Create an new packet and store the cost from node0 to others in the `packet.mincost[]`.
- Use a for loop to set the packet's destination and use `tolayer2()` to send it out.
```{.c .numberLines}
extern void rtinit0() 
{
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
```

#### rtupdate0(struct rtpkt *rcvdpkt)
- Declare the variables which can record the status of changed and the source of the received packet.
- Copy the sender's shortest distance to each node to the `tempCost[]`.
- Construct an array(`old_shortest[]`) which records the shortest distance from the current node to each node.
- Re-calculate the shortest path to each node by applying the concept of **Bellman-Ford** algorithm.
- Construct an new array(`shortest[]`) which records the shortest disance after the process that re-calculating the shortest distance.
- Compare the old shortest distance and the new shortest distance.
    - Exists any differences: set the `changed` to 1 in order to notify that we need to send the update packet to our neighbors for later process.
    - No differences: No need to send the update packet since the shortest distance to each node remain the same.
```{.c .numberLines}
extern void rtupdate0(struct rtpkt *rcvdpkt)
{
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
```

#### linkhandler0(int linkid, int newcost)
- Record the oldcost since we need to update the cost of current node in later process.
- Since only an edge's weight changed, we only need to update the cost related to that edge by using the equation `dt0.costs[i][linkid] - oldcost + newcost`.
- Store the new cost to weight of edge between the current node and the node{linkid}.
- Construct a array(`shortest`) which records the shortest distance from the current node to each node.
- Prepare for the update packet which is going to be sent to other nodes, including `packet.mincost[] = shortest[]` and setting the `packet.sourceid` and `packet.destid`.
- Send the update packet out by invoking `tolayer2()`.
```{.c .numberLines}
extern void linkhandler0(int linkid, int newcost)
{
  int oldcost = dt0.costs[1][1];

  for (int i = 0; i < 4; i++) {
    int newValue = dt0.costs[i][linkid] - oldcost + newcost;
    dt0.costs[i][linkid] = newValue;
  }

  dt0.costs[1][1] = newcost;


  // construct a shortest array
  int min = 999;
  int shortest[4] = {999, 999, 999, 999};
  shortest[0] = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {

      if(dt0.costs[i][j] < min && dt0.costs[i][j] >= 0)
        min = dt0.costs[i][j];

    }
    shortest[i] = min;
    min = 999;
  }

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
```

### Problems
- Part2 is the part that I spend most of the time in this Assignment.
- I encoutered some bugs when the edge's weight changed, I thought the mistake is from the `linkhandler()`, but after discussing with the classmates, I modify my `rtupdate()` to meet the requirements.
- Before modifying, I didn't construct `old_shortest` and `shortest` which are the old shortest distance array and the new shortest distance array. By fixing that, I can compare the differences between the two array and determine whether I should send the update packet to other nodes or not.

## Part 3

### (1) What is the time complexity of Dijkstra’s algorithm? Please briefly describe the steps of the algorithm to justify the time complexity. 
- My implementation of Dijkstra's algorithm is an greedy algorithm.
- I used an nested loop to find out the closest node to the source node and use that node to update the shortest distance to others.
- The procedure above indicate that the time-complexity of Dijkstra algorithm in my implementation is ![img](https://latex.codecogs.com/svg.image?O(|V|^2)), where ![img](https://latex.codecogs.com/svg.image?|V|) is the number of vertices.
- However, if we use Fibonacci heap to deal with the problem, the time complexity can be reduced to ![img](https://latex.codecogs.com/svg.image?O((|V|+|E|)log|V|)), where ![img](https://latex.codecogs.com/svg.image?|V|) is the number of the vertices and ![img](https://latex.codecogs.com/svg.image?|E|) is the number of edges.

### (2) What is the time complexity of Bellman-Ford algorithm? Please briefly describe the steps of the algorithm to justify the time complexity.
- In Bellman-Ford algorithm, we need to initialize the shortest distance array to each node.
- Then apply a nested for loop to traverse each node with each edge and update the cost.
- Thus the time-complexity is ![img](https://latex.codecogs.com/svg.image?O(|V|*|E|)), where ![img](https://latex.codecogs.com/svg.image?|V|) is the number of the vertices and ![img](https://latex.codecogs.com/svg.image?|E|) is the number of edges.

### (3) How does the distance vector routing algorithm send routing packets? (To all nodes or only to neighbors)

- Distance Vector Routing Algorithm send routing packet only to its neighbors.
- The communicating with the neighbors takes place at regular intervals.
- The router shares routing table with its neighbors and update the table according to the neighbors' table.
- Moreover, it often uses Bellman-Ford algorithm for making routing table and UDP for transportation.

### (4) How does the link state routing algorithm send routing packets? (To all nodes or only to neighbors)

- Link State Routing Algorithm send routing packet to all nodes.
- The communicating with the neighbors takes place whenever there is a change.
- The router sends its table only to all the routers through flooding.
- Moreover, if often uses Dijkstra's algorithm for making routing table and TCP/HTTP for transportation.


### (5) When a link cost changes, which steps does the distance vector algorithm take?
- Once the distance changed, the router will update its routing table and correct the cost of the shortest distance to each router.
- Then it will construct a packet which contains all the shortest distance to other nodes and use UDP send this packet to the neighbor routers in order to share the information(routing table).