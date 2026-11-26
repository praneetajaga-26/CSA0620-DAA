#include <stdio.h>
#define MAX 20
#define INF 1000000000

int graph[MAX][MAX], dist[MAX], parent[MAX], used[MAX];

void dijkstra(int n, int src) {
    int i, j, u, best;
    for (i=0;i<n;i++) { dist[i]=INF; parent[i]=-1; used[i]=0; }
    dist[src]=0;
    for (i=0;i<n;i++) {
        u=-1; best=INF;
        for (j=0;j<n;j++)
            if (!used[j] && dist[j]<best) { best=dist[j]; u=j; }
        if (u==-1) break;
        used[u]=1;
        for (j=0;j<n;j++)
            if (graph[u][j]>0 && !used[j] &&
                dist[u]+graph[u][j]<dist[j]) {
                dist[j]=dist[u]+graph[u][j];
                parent[j]=u;
            }
    }
}

void printPath(int dest) {
    int p[MAX], count=0, i;
    while (dest!=-1 && count<MAX) {
        p[count++]=dest;
        dest=parent[dest];
    }
    for (i=count-1;i>=0;i--) {
        printf("%d",p[i]+1);
        if (i) printf(" -> ");
    }
}

int fixedOrderDistance(int n, int collected[]) {
    int current=0, total=0, i;
    for (i=1;i<n-1;i++) if (collected[i]) {
        dijkstra(n,current);
        if (dist[i]==INF) return INF;
        total+=dist[i]; current=i;
    }
    dijkstra(n,current);
    if (dist[n-1]==INF) return INF;
    return total+dist[n-1];
}

int main(void) {
    int n,e,i,j,u,v,w,capacity,remaining,totalWaste=0;
    int waste[MAX]={0}, collected[MAX]={0};
    int current=0,next, best, route=0, count=0, baseline;
    int reached=0;

    printf("=== WASTE COLLECTION ROUTE OPTIMIZATION ===\\n");
    printf("Enter total locations (3-%d): ",MAX);
    if (scanf("%d",&n)!=1 || n<3 || n>MAX) {
        printf("Invalid number of locations.\\n"); return 1;
    }

    printf("\\nLocation 1: Depot\\n");
    for(i=1;i<n-1;i++) printf("Location %d: Waste bin %d\\n",i+1,i);
    printf("Location %d: Recycling centre\\n",n);

    printf("\\nEnter number of roads: ");
    if(scanf("%d",&e)!=1 || e<1 || e>n*(n-1)/2) {
        printf("Invalid number of roads.\\n"); return 1;
    }
    for(i=0;i<n;i++) for(j=0;j<n;j++) graph[i][j]=0;

    printf("Enter roads as: location1 location2 distance_km\\n");
    for(i=0;i<e;i++) {
        printf("Road %d: ",i+1);
        if(scanf("%d %d %d",&u,&v,&w)!=3 ||
           u<1 || u>n || v<1 || v>n || u==v || w<=0) {
            printf("Invalid road details.\\n"); return 1;
        }
        graph[u-1][v-1]=w; graph[v-1][u-1]=w;
    }

    printf("\\nEnter waste quantity at each bin (kg):\\n");
    for(i=1;i<n-1;i++) {
        printf("Waste at location %d: ",i+1);
        if(scanf("%d",&waste[i])!=1 || waste[i]<0) {
            printf("Invalid waste quantity.\\n"); return 1;
        }
        totalWaste+=waste[i];
    }
    printf("Enter truck capacity (kg): ");
    if(scanf("%d",&capacity)!=1 || capacity<=0) {
        printf("Invalid capacity.\\n"); return 1;
    }
    remaining=capacity;

    printf("\\n--- OPTIMIZED COLLECTION ROUTE ---\\n");
    printf("Start: Depot (Location 1)\\n");

    while(1) {
        dijkstra(n,current);
        next=-1; best=INF;
        for(i=1;i<n-1;i++)
            if(!collected[i] && waste[i]>0 &&
               waste[i]<=remaining && dist[i]<best) {
                best=dist[i]; next=i;
            }
        if(next==-1) break;

        printf("\\nVisit bin at Location %d\\n",next+1);
        printf("Path: "); printPath(next);
        printf("\\nTravel distance: %d km\\n",best);
        printf("Waste collected: %d kg\\n",waste[next]);

        route+=best;
        remaining-=waste[next];
        collected[next]=1;
        count++;
        current=next;
    }

    dijkstra(n,current);
    if(dist[n-1]!=INF) {
        printf("\\nGo to recycling centre\\nPath: ");
        printPath(n-1);
        printf("\\nTravel distance: %d km\\n",dist[n-1]);
        route+=dist[n-1];
        reached=1;
    } else {
        printf("\\nWARNING: Recycling centre is unreachable.\\n");
    }

    /* Ignore empty bins in the fixed-order comparison. */
    for(i=1;i<n-1;i++) if(waste[i]==0) collected[i]=1;
    baseline=fixedOrderDistance(n,collected);

    printf("\\n========== FINAL REPORT ==========\\n");
    printf("Bins containing waste: %d\\n",n-2);
    printf("Bins visited: %d\\n",count);
    printf("Total waste available: %d kg\\n",totalWaste);
    printf("Waste collected: %d kg\\n",capacity-remaining);
    printf("Truck capacity: %d kg\\n",capacity);
    printf("Optimized route distance: %d km\\n",route);

    if(reached && baseline!=INF) {
        int saved=baseline-route;
        printf("Fixed-order route distance: %d km\\n",baseline);
        if(saved>0)
            printf("Distance saved: %d km (%.2f%%)\\n",
                   saved,saved*100.0f/baseline);
        else
            printf("Distance saved: 0 km (greedy route was not shorter).\\n");
    } else printf("Route comparison unavailable.\\n");

    printf("\\nUncollected bins:\\n");
    {
        int found=0;
        for(i=1;i<n-1;i++) if(waste[i]>0 && !collected[i]) {
            printf("Location %d: %d kg (capacity or route limitation)\\n",
                   i+1,waste[i]);
            found=1;
        }
        if(!found) printf("None\\n");
    }
    printf("\\nSystem execution completed.\\n");
    return 0;
}
