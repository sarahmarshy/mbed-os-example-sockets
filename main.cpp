#include "mbed.h"
#include "TCPSocket.h"
#include "mbed_stats.h"

#define STR(x) STR2(x)
#define STR2(x) #x


// Socket demo
void http_demo(NetworkInterface *net) {
    TCPSocket socket;

    // Show the network address
    const char *ip = net->get_ip_address();
    printf("IP address is: %s\n", ip ? ip : "No IP");

    // Open a socket on the network interface, and create a TCP connection to mbed.org
    socket.open(net);
    socket.connect("2a00:1450:4013:c00::6a", 80);//google.com

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: developer.mbed.org\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\r\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}


#include "NanostackInterface.h"
LoWPANNDInterface mesh;

int main() {
    // Brings up the mesh
    printf("6Lowpan example\n");
    if (mesh.connect()) {
        printf("Connection failed!\n");
        return -1;
    }

    // Invoke the demo
    http_demo(&mesh);

    // Brings down the mesh
    mesh.disconnect();

    printf("Done\n");
    mbed_stats_heap_t stats;
    mbed_stats_heap_get(&stats);
    printf("Heap metrics\n");
    printf(" Allocation count: %u\n", stats.alloc_cnt);
    printf(" Max size: %u\n", stats.max_size);
    printf(" Current size: %u\n", stats.current_size);
    printf(" Total size allocated: %u\n", stats.total_size);
    printf(" Allocation fail count: %u\n", stats.alloc_fail_cnt);
}

// Example with the ESP8266 interface
#if defined(MBED_DEMO_WIFI)
#include "ESP8266Interface.h"

ESP8266Interface wifi(D1, D0);

int main() {
    // Brings up the esp8266
    printf("ESP8266 socket example\n");
    wifi.connect(STR(MBED_DEMO_WIFI_SSID), STR(MBED_DEMO_WIFI_PASS));

    // Invoke the demo
    http_demo(&wifi);

    // Brings down the esp8266 
    wifi.disconnect();

    printf("Done\n");
}

// Example using the builtin ethernet interface
#elif 0
#include "EthernetInterface.h"

EthernetInterface eth;

int main() {
    // Brings up the ethernet interface
    printf("Ethernet socket example\n");
    eth.connect();

    // Invoke the demo
    http_demo(&eth);

    // Brings down the ethernet interface
    eth.disconnect();

    printf("Done\n");
}
#endif
