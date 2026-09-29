#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define MAX_NODES 30
#define MAX_VEHICLES 20
#define MAX_EMERGENCIES 50
#define MAX_HEAP 100
#define TEXT 50

typedef struct
{
    int id;
    char name[TEXT];
    char type[25];
} Location;

typedef struct Road
{
    int to;
    float distance;
    int traffic;
    int condition;
    float cost;
    struct Road *next;
} Road;

typedef struct
{
    int id;
    char type[25];
    int location;
    int available;
} Vehicle;

typedef struct
{
    int id;
    int vehicleId;
    int source;
    int destination;
    int priority;
} Emergency;

typedef struct
{
    int node;
    float value;
} HeapItem;

typedef struct
{
    HeapItem data[MAX_HEAP];
    int size;
} MinHeap;

Location places[MAX_NODES];
Road *network[MAX_NODES];
Vehicle fleet[MAX_VEHICLES];
Emergency requests[MAX_EMERGENCIES];

int placeCount = 0;
int vehicleCount = 0;
int requestCount = 0;

float trainDistance[] =
{
    2, 4, 5, 7, 9,
    11, 13, 15, 18, 20,
    23, 26
};

float trainTraffic[] =
{
    1, 1, 2, 2, 3,
    2, 3, 4, 2, 4,
    3, 4
};

float trainCondition[] =
{
    1, 1, 1, 2, 1,
    2, 2, 1, 1, 2,
    2, 3
};

float trainTimeOfDay[] =
{
    1, 1, 2, 2, 3,
    2, 3, 3, 1, 3,
    2, 3
};

float trainTravelTime[] =
{
    5, 9, 12, 17, 22,
    25, 31, 35, 38, 48,
    51, 61
};

int trainingCount = 12;

void initializeSystem();
void addLocation();
void showLocations();
void addRoad();
void showRoadNetwork();
void registerVehicle();
void showVehicles();
void createEmergency();
void showEmergencies();
void heapInitialize(MinHeap *heap);
void heapInsert(MinHeap *heap, int node, float value);
HeapItem heapRemove(MinHeap *heap);
void heapUp(MinHeap *heap, int index);
void heapDown(MinHeap *heap, int index);
void swapHeap(HeapItem *a, HeapItem *b);
float trafficMultiplier(int level);
float conditionMultiplier(int condition);
void shortestRoute(int source, int destination);
void printRoute(int parent[], int node);
void routePlanner();
void compareRoutes();
void processEmergency();

void trainAI(float *b0,
             float *b1,
             float *b2,
             float *b3,
             float *b4);

float predictTime(float distance,
                  float traffic,
                  float condition,
                  float timeOfDay);

void aiTravelPrediction();
void saveProject();
void loadDemoData();
void menu();

void initializeSystem()
{
    int i;

    for (i = 0; i < MAX_NODES; i++)
    {
        network[i] = NULL;
    }
}

float trafficMultiplier(int level)
{
    if (level == 1)
        return 1.0;

    if (level == 2)
        return 1.35;

    if (level == 3)
        return 1.80;

    if (level == 4)
        return 2.50;

    return 1.0;
}

float conditionMultiplier(int condition)
{
    if (condition == 1)
        return 1.0;

    if (condition == 2)
        return 1.20;

    if (condition == 3)
        return 1.45;

    return 1.0;
}

void addLocation()
{
    if (placeCount >= MAX_NODES)
    {
        printf("\nLocation capacity reached.\n");
        return;
    }

    places[placeCount].id = placeCount;

    printf("\nEnter location name: ");
    scanf(" %49[^\n]", places[placeCount].name);

    printf("Enter location type:\n");
    printf("1. Hospital\n");
    printf("2. Fire Station\n");
    printf("3. Junction\n");
    printf("4. Emergency Area\n");

    int type;
    scanf("%d", &type);

    if (type == 1)
        strcpy(places[placeCount].type, "Hospital");
    else if (type == 2)
        strcpy(places[placeCount].type, "Fire Station");
    else if (type == 3)
        strcpy(places[placeCount].type, "Junction");
    else
        strcpy(places[placeCount].type, "Emergency Area");

    printf("\nLocation created successfully.");
    printf("\nAssigned ID: %d\n", placeCount);

    placeCount++;
}

void showLocations()
{
    int i;
    printf("\n========== LOCATIONS ==========\n");

    if (placeCount == 0)
    {
        printf("No locations have been added.\n");
        return;
    }

    for (i = 0; i < placeCount; i++)
    {
        printf("[%d] %-25s | %s\n",
               places[i].id,
               places[i].name,
               places[i].type);
    }
}

void addRoad()
{
    int from;
    int to;
    int traffic;
    int condition;
    float distance;
    float cost;
    Road *a;
    Road *b;

    if (placeCount < 2)
    {
        printf("\nCreate at least two locations first.\n");
        return;
    }

    showLocations();

    printf("\nEnter starting location ID: ");
    scanf("%d", &from);

    printf("Enter destination location ID: ");
    scanf("%d", &to);

    if (from < 0 ||
        from >= placeCount ||
        to < 0 ||
        to >= placeCount ||
        from == to)
    {
        printf("\nInvalid location selection.\n");
        return;
    }

    printf("Enter distance in kilometres: ");
    scanf("%f", &distance);

    printf("\nTraffic:\n");
    printf("1 - Low\n");
    printf("2 - Moderate\n");
    printf("3 - Heavy\n");
    printf("4 - Severe\n");

    printf("Select traffic level: ");
    scanf("%d", &traffic);

    printf("\nRoad condition:\n");
    printf("1 - Good\n");
    printf("2 - Average\n");
    printf("3 - Poor\n");

    printf("Select road condition: ");
    scanf("%d", &condition);

    if (traffic < 1 || traffic > 4 ||
        condition < 1 || condition > 3 ||
        distance <= 0)
    {
        printf("\nInvalid road information.\n");
        return;
    }

    cost = distance *
           trafficMultiplier(traffic) *
           conditionMultiplier(condition);


    a = (Road *)malloc(sizeof(Road));

    if (a == NULL)
    {
        printf("\nMemory allocation error.\n");
        return;
    }

    a->to = to;
    a->distance = distance;
    a->traffic = traffic;
    a->condition = condition;
    a->cost = cost;

    a->next = network[from];
    network[from] = a;


    b = (Road *)malloc(sizeof(Road));

    if (b == NULL)
    {
        printf("\nMemory allocation error.\n");
        return;
    }

    b->to = from;
    b->distance = distance;
    b->traffic = traffic;
    b->condition = condition;
    b->cost = cost;

    b->next = network[to];
    network[to] = b;


    printf("\nRoad successfully added.\n");
    printf("Physical distance : %.2f km\n", distance);
    printf("Traffic factor    : %.2f\n",
           trafficMultiplier(traffic));
    printf("Condition factor  : %.2f\n",
           conditionMultiplier(condition));
    printf("Route cost        : %.2f\n", cost);
}

void showRoadNetwork()
{
    int i;

    Road *road;

    printf("\n========== ROAD NETWORK ==========\n");

    for (i = 0; i < placeCount; i++)
    {
        printf("\n%s\n", places[i].name);

        road = network[i];

        if (road == NULL)
        {
            printf("  No connected roads\n");
            continue;
        }

        while (road != NULL)
        {
            printf("  -> %s | %.1f km | traffic %d | condition %d | cost %.2f\n",
                   places[road->to].name,
                   road->distance,
                   road->traffic,
                   road->condition,
                   road->cost);

            road = road->next;
        }
    }
}

void registerVehicle()
{
    if (vehicleCount >= MAX_VEHICLES)
    {
        printf("\nVehicle capacity reached.\n");
        return;
    }

    printf("\nEnter vehicle ID: ");
    scanf("%d", &fleet[vehicleCount].id);

    printf("Vehicle type:\n");
    printf("1. Ambulance\n");
    printf("2. Fire Engine\n");
    printf("3. Rescue Vehicle\n");

    int choice;
    scanf("%d", &choice);

    if (choice == 1)
        strcpy(fleet[vehicleCount].type, "Ambulance");
    else if (choice == 2)
        strcpy(fleet[vehicleCount].type, "Fire Engine");
    else
        strcpy(fleet[vehicleCount].type, "Rescue Vehicle");

    showLocations();

    printf("\nCurrent location ID: ");
    scanf("%d", &fleet[vehicleCount].location);

    if (fleet[vehicleCount].location < 0 ||
        fleet[vehicleCount].location >= placeCount)
    {
        printf("\nInvalid location.\n");
        return;
    }

    fleet[vehicleCount].available = 1;

    printf("\nEmergency vehicle registered.\n");

    vehicleCount++;
}

void showVehicles()
{
    int i;

    printf("\n========== EMERGENCY VEHICLES ==========\n");

    if (vehicleCount == 0)
    {
        printf("No vehicles registered.\n");
        return;
    }

    for (i = 0; i < vehicleCount; i++)
    {
        printf("\nID       : %d", fleet[i].id);
        printf("\nType     : %s", fleet[i].type);
        printf("\nLocation : %s",
               places[fleet[i].location].name);
        printf("\nStatus   : %s\n",
               fleet[i].available
               ? "Available"
               : "Busy");
    }
}

void createEmergency()
{
    Emergency *e;

    if (requestCount >= MAX_EMERGENCIES)
    {
        printf("\nEmergency queue capacity reached.\n");
        return;
    }

    e = &requests[requestCount];

    printf("\nEnter emergency request ID: ");
    scanf("%d", &e->id);

    showVehicles();

    printf("\nAssign vehicle ID: ");
    scanf("%d", &e->vehicleId);

    showLocations();

    printf("\nEnter emergency source ID: ");
    scanf("%d", &e->source);

    printf("Enter destination ID: ");
    scanf("%d", &e->destination);

    if (e->source < 0 ||
        e->source >= placeCount ||
        e->destination < 0 ||
        e->destination >= placeCount)
    {
        printf("\nInvalid location.\n");
        return;
    }

    printf("\nEmergency severity:\n");
    printf("1 - Critical\n");
    printf("2 - High\n");
    printf("3 - Medium\n");
    printf("4 - Low\n");

    printf("Enter severity: ");
    scanf("%d", &e->priority);

    if (e->priority < 1 || e->priority > 4)
    {
        printf("\nInvalid priority.\n");
        return;
    }

    requestCount++;

    printf("\nEmergency request recorded.\n");
}

void showEmergencies()
{
    int i;

    printf("\n========== ACTIVE EMERGENCIES ==========\n");

    if (requestCount == 0)
    {
        printf("No active emergency requests.\n");
        return;
    }

    for (i = 0; i < requestCount; i++)
    {
        printf("\nRequest ID : %d", requests[i].id);

        printf("\nVehicle ID : %d", requests[i].vehicleId);

        printf("\nFrom       : %s",
               places[requests[i].source].name);

        printf("\nTo         : %s",
               places[requests[i].destination].name);

        printf("\nPriority   : %d\n",
               requests[i].priority);
    }
}

void heapInitialize(MinHeap *heap)
{
    heap->size = 0;
}

void swapHeap(HeapItem *a,
              HeapItem *b)
{
    HeapItem temp = *a;

    *a = *b;
    *b = temp;
}


void heapUp(MinHeap *heap,
            int index)
{
    int parent;

    while (index > 0)
    {
        parent = (index - 1) / 2;

        if (heap->data[parent].value <=
            heap->data[index].value)
            break;

        swapHeap(&heap->data[parent],
                 &heap->data[index]);

        index = parent;
    }
}


void heapDown(MinHeap *heap,int index)
{
    int left;
    int right;
    int smallest;

    while (1)
    {
        left = index * 2 + 1;
        right = index * 2 + 2;

        smallest = index;

        if (left < heap->size &&
            heap->data[left].value <
            heap->data[smallest].value)
        {
            smallest = left;
        }

        if (right < heap->size &&
            heap->data[right].value <
            heap->data[smallest].value)
        {
            smallest = right;
        }

        if (smallest == index)
            break;

        swapHeap(&heap->data[index],
                 &heap->data[smallest]);

        index = smallest;
    }
}


void heapInsert(MinHeap *heap,int node,float value)
{
    if (heap->size >= MAX_HEAP)
        return;

    heap->data[heap->size].node = node;
    heap->data[heap->size].value = value;

    heapUp(heap, heap->size);

    heap->size++;
}

HeapItem heapRemove(MinHeap *heap)
{
    HeapItem result;

    result.node = -1;
    result.value = FLT_MAX;

    if (heap->size == 0)
        return result;

    result = heap->data[0];

    heap->size--;

    if (heap->size > 0)
    {
        heap->data[0] =
            heap->data[heap->size];

        heapDown(heap, 0);
    }

    return result;
}

void printRoute(int parent[],int node)
{
    if (parent[node] == -1)
    {
        printf("%s", places[node].name);
        return;
    }

    printRoute(parent, parent[node]);

    printf(" -> %s",
           places[node].name);
}

void shortestRoute(int source,int destination)
{
    float distance[MAX_NODES];

    int parent[MAX_NODES];

    int visited[MAX_NODES];

    int i;

    MinHeap heap;

    HeapItem item;

    Road *road;

    float alternative;

    for (i = 0; i < placeCount; i++)
    {
        distance[i] = FLT_MAX;
        parent[i] = -1;
        visited[i] = 0;
    }


    heapInitialize(&heap);

    distance[source] = 0;

    heapInsert(&heap,source,0);

    while (heap.size > 0)
    {
        item = heapRemove(&heap);

        if (item.node == -1)
            break;

        if (visited[item.node])
            continue;

        visited[item.node] = 1;

        if (item.node == destination)
            break;

        road = network[item.node];

        while (road != NULL)
        {
            alternative =
                distance[item.node] +
                road->cost;

            if (!visited[road->to] &&
                alternative <
                distance[road->to])
            {
                distance[road->to] =
                    alternative;

                parent[road->to] =
                    item.node;

                heapInsert(
                    &heap,
                    road->to,
                    alternative
                );
            }

            road = road->next;
        }
    }


    if (distance[destination] == FLT_MAX)
    {
        printf("\nNo reachable route found.\n");
        return;
    }


    printf("\n========== ROUTE ANALYSIS ==========\n");

    printf("\nSelected route:\n");

    printRoute(parent,
               destination);

    printf("\n\nTraffic-adjusted route cost: %.2f\n",
           distance[destination]);
}

void routePlanner()
{
    int source;
    int destination;

    showLocations();

    printf("\nSource location ID: ");
    scanf("%d", &source);

    printf("Destination location ID: ");
    scanf("%d", &destination);

    if (source < 0 ||
        source >= placeCount ||
        destination < 0 ||
        destination >= placeCount)
    {
        printf("\nInvalid location.\n");
        return;
    }

    shortestRoute(source,destination);
}

void compareRoutes()
{
    int source;
    int destination;

    Road *road;

    int found = 0;

    showLocations();

    printf("\nSource ID: ");
    scanf("%d", &source);

    printf("Destination ID: ");
    scanf("%d", &destination);

    if (source < 0 ||
        source >= placeCount ||
        destination < 0 ||
        destination >= placeCount)
    {
        printf("\nInvalid location.\n");
        return;
    }


    printf("\n========== AVAILABLE DIRECT OPTIONS ==========\n");

    road = network[source];

    while (road != NULL)
    {
        if (road->to == destination)
        {
            printf("\nDirect road found.");
            printf("\nDistance : %.2f km",
                   road->distance);
            printf("\nTraffic  : %d",
                   road->traffic);
            printf("\nCondition: %d",
                   road->condition);
            printf("\nCost     : %.2f\n",
                   road->cost);

            found = 1;
        }

        road = road->next;
    }


    if (!found)
    {
        printf("\nNo direct road exists.");
    }


    printf("\n\n========== NETWORK OPTIMIZED ROUTE ==========\n");

    shortestRoute(source,
                  destination);
}

void processEmergency()
{
    int selected = -1;

    int bestPriority = 999;

    int i;

    if (requestCount == 0)
    {
        printf("\nNo emergency requests waiting.\n");
        return;
    }


    for (i = 0; i < requestCount; i++)
    {
        if (requests[i].priority <
            bestPriority)
        {
            bestPriority =
                requests[i].priority;

            selected = i;
        }
    }


    printf("\n============================================\n");
    printf("          EMERGENCY DISPATCH\n");
    printf("============================================\n");

    printf("\nRequest ID : %d",
           requests[selected].id);

    printf("\nVehicle ID : %d",
           requests[selected].vehicleId);

    printf("\nPriority   : %d",
           requests[selected].priority);

    printf("\nSource     : %s",
           places[requests[selected].source].name);

    printf("\nDestination: %s\n",
           places[requests[selected].destination].name);


    printf("\nFinding traffic-aware route...\n");

    shortestRoute(
        requests[selected].source,
        requests[selected].destination
    );

    for (i = selected;i < requestCount - 1;i++)
    {
        requests[i] =
            requests[i + 1];
    }

    requestCount--;

    printf("\nEmergency dispatched successfully.\n");
}




void trainAI(float *b0,float *b1,float *b2,float *b3,float *b4)
{
    float w0 = 0;
    float w1 = 1.8;
    float w2 = 2.0;
    float w3 = 1.0;
    float w4 = 1.0;

    float learningRate = 0.0001;

    int epoch;
    int i;

    float prediction;
    float error;


    for (epoch = 0; epoch < 15000; epoch++)
    {
        float g0 = 0;
        float g1 = 0;
        float g2 = 0;
        float g3 = 0;
        float g4 = 0;


        for (i = 0; i < trainingCount; i++)
        {
            prediction =
                w0 +
                w1 * trainDistance[i] +
                w2 * trainTraffic[i] +
                w3 * trainCondition[i] +
                w4 * trainTimeOfDay[i];


            error =
                prediction -
                trainTravelTime[i];


            g0 += error;
            g1 += error * trainDistance[i];
            g2 += error * trainTraffic[i];
            g3 += error * trainCondition[i];
            g4 += error * trainTimeOfDay[i];
        }


        w0 -= learningRate * g0;
        w1 -= learningRate * g1;
        w2 -= learningRate * g2;
        w3 -= learningRate * g3;
        w4 -= learningRate * g4;
    }


    *b0 = w0;
    *b1 = w1;
    *b2 = w2;
    *b3 = w3;
    *b4 = w4;
}

float predictTime(float distance,float traffic,float condition,float timeOfDay)
{
    float b0;
    float b1;
    float b2;
    float b3;
    float b4;

    trainAI(
        &b0,
        &b1,
        &b2,
        &b3,
        &b4
    );


    return b0 +
           b1 * distance +
           b2 * traffic +
           b3 * condition +
           b4 * timeOfDay;
}

void aiTravelPrediction()
{
    float distance;
    float traffic;
    float condition;
    float timeOfDay;

    float result;


    printf("\n========== AI TRAVEL-TIME PREDICTION ==========\n");

    printf("\nDistance in kilometres: ");
    scanf("%f", &distance);

    printf("\nTraffic level");
    printf("\n1 - Low");
    printf("\n2 - Moderate");
    printf("\n3 - Heavy");
    printf("\n4 - Severe");
    printf("\nSelect: ");
    scanf("%f", &traffic);


    printf("\nRoad condition");
    printf("\n1 - Good");
    printf("\n2 - Average");
    printf("\n3 - Poor");
    printf("\nSelect: ");
    scanf("%f", &condition);


    printf("\nTime of day");
    printf("\n1 - Normal");
    printf("\n2 - Busy");
    printf("\n3 - Peak");
    printf("\nSelect: ");
    scanf("%f", &timeOfDay);


    if (distance <= 0 ||
        traffic < 1 || traffic > 4 ||
        condition < 1 || condition > 3 ||
        timeOfDay < 1 || timeOfDay > 3)
    {
        printf("\nInvalid prediction inputs.\n");
        return;
    }


    result = predictTime(distance,traffic,condition,timeOfDay);

    if (result < 0)
        result = 0;


    printf("\nAI prediction result:");
    printf("\nEstimated travel time: %.2f minutes\n",
           result);
}

void saveProject()
{
    FILE *file;

    int i;

    file = fopen("emergency_project_data.txt","w");

    if (file == NULL)
    {
        printf("\nUnable to create data file.\n");
        return;
    }


    fprintf(file,"EMERGENCY ROUTE PLANNER\n");

    fprintf(file,"Locations=%d\n",placeCount);

    for (i = 0; i < placeCount; i++)
    {
        fprintf(file,
                "%d|%s|%s\n",
                places[i].id,
                places[i].name,
                places[i].type);
    }


    fprintf(file,
            "Vehicles=%d\n",
            vehicleCount);


    for (i = 0; i < vehicleCount; i++)
    {
        fprintf(file,
                "%d|%s|%d|%d\n",
                fleet[i].id,
                fleet[i].type,
                fleet[i].location,
                fleet[i].available);
    }


    fprintf(file,
            "Emergencies=%d\n",
            requestCount);


    for (i = 0; i < requestCount; i++)
    {
        fprintf(file,
                "%d|%d|%d|%d|%d\n",
                requests[i].id,
                requests[i].vehicleId,
                requests[i].source,
                requests[i].destination,
                requests[i].priority);
    }


    fclose(file);

    printf("\nProject information saved successfully.\n");
}

void loadDemoData()
{
    placeCount = 0;
    vehicleCount = 0;
    requestCount = 0;

    initializeSystem();


    strcpy(places[0].name,
           "Central Junction");

    strcpy(places[0].type,
           "Junction");


    strcpy(places[1].name,
           "City Hospital");

    strcpy(places[1].type,
           "Hospital");


    strcpy(places[2].name,
           "Fire Station");

    strcpy(places[2].type,
           "Fire Station");


    strcpy(places[3].name,
           "Market Area");

    strcpy(places[3].type,
           "Junction");


    strcpy(places[4].name,
           "Railway Station");

    strcpy(places[4].type,
           "Junction");


    strcpy(places[5].name,
           "Industrial Area");

    strcpy(places[5].type,
           "Emergency Area");


    placeCount = 6;


    int i;

    for (i = 0; i < placeCount; i++)
    {
        places[i].id = i;
    }



    Road *r;


    

    r = malloc(sizeof(Road));

    r->to = 1;
    r->distance = 5;
    r->traffic = 2;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[0];
    network[0] = r;


    r = malloc(sizeof(Road));

    r->to = 0;
    r->distance = 5;
    r->traffic = 2;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[1];
    network[1] = r;


  

    r = malloc(sizeof(Road));

    r->to = 2;
    r->distance = 4;
    r->traffic = 1;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[0];
    network[0] = r;


    r = malloc(sizeof(Road));

    r->to = 0;
    r->distance = 4;
    r->traffic = 1;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[2];
    network[2] = r;
  
    r = malloc(sizeof(Road));

    r->to = 3;
    r->distance = 3;
    r->traffic = 3;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[1];
    network[1] = r;


    r = malloc(sizeof(Road));

    r->to = 1;
    r->distance = 3;
    r->traffic = 3;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[3];
    network[3] = r;

    r = malloc(sizeof(Road));

    r->to = 4;
    r->distance = 6;
    r->traffic = 2;
    r->condition = 2;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[2];
    network[2] = r;


    r = malloc(sizeof(Road));

    r->to = 2;
    r->distance = 6;
    r->traffic = 2;
    r->condition = 2;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[4];
    network[4] = r;


    r = malloc(sizeof(Road));

    r->to = 5;
    r->distance = 5;
    r->traffic = 2;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[3];
    network[3] = r;


    r = malloc(sizeof(Road));

    r->to = 3;
    r->distance = 5;
    r->traffic = 2;
    r->condition = 1;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[5];
    network[5] = r;



    r = malloc(sizeof(Road));

    r->to = 5;
    r->distance = 4;
    r->traffic = 4;
    r->condition = 2;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[4];
    network[4] = r;


    r = malloc(sizeof(Road));

    r->to = 4;
    r->distance = 4;
    r->traffic = 4;
    r->condition = 2;

    r->cost =
        r->distance *
        trafficMultiplier(r->traffic) *
        conditionMultiplier(r->condition);

    r->next = network[5];
    network[5] = r;


    fleet[0].id = 101;

    strcpy(fleet[0].type,
           "Ambulance");

    fleet[0].location = 0;

    fleet[0].available = 1;

    vehicleCount = 1;


    printf("\nDemo network loaded successfully.\n");
}

void menu()
{
    printf("\n\n");
    printf("====================================================\n");
    printf("       AI-BASED EMERGENCY VEHICLE ROUTE PLANNER\n");
    printf("====================================================\n");

    printf(" 1. Add Location\n");
    printf(" 2. View Locations\n");
    printf(" 3. Add Road\n");
    printf(" 4. View Road Network\n");

    printf(" 5. Register Emergency Vehicle\n");
    printf(" 6. View Emergency Vehicles\n");

    printf(" 7. Create Emergency Request\n");
    printf(" 8. View Emergency Requests\n");
    printf(" 9. Process Emergency\n");

    printf("10. Find Shortest Route\n");
    printf("11. Compare Routes\n");

    printf("12. AI Travel-Time Prediction\n");

    printf("13. Save Project Data\n");
    printf("14. Load Demonstration Data\n");

    printf("15. Exit\n");

    printf("====================================================\n");
}

int main()
{
    int choice;

    initializeSystem();

    printf("\n");
    printf("====================================================\n");
    printf("       EMERGENCY ROUTE PLANNER SYSTEM\n");
    printf("====================================================\n");

    printf("\nC-Based DSA + AI Demonstration Project\n");


    while (1)
    {
        menu();

        printf("\nEnter your choice: ");
        scanf("%d", &choice);


        switch (choice)
        {
            case 1:
                addLocation();
                break;

            case 2:
                showLocations();
                break;

            case 3:
                addRoad();
                break;

            case 4:
                showRoadNetwork();
                break;

            case 5:
                registerVehicle();
                break;

            case 6:
                showVehicles();
                break;

            case 7:
                createEmergency();
                break;

            case 8:
                showEmergencies();
                break;

            case 9:
                processEmergency();
                break;

            case 10:
                routePlanner();
                break;

            case 11:
                compareRoutes();
                break;

            case 12:
                aiTravelPrediction();
                break;

            case 13:
                saveProject();
                break;

            case 14:
                loadDemoData();
                break;

            case 15:
                printf("\nProject terminated.\n");
                return 0;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    }
    return 0;
}