#include <fastnet/sigflow.h>
#include <time.h>

int main() {
    auto flow = fnet::sigflow::instance();
    flow->add_signal(SIGINT, [](){ std::cout<<"got\n";} );
    int count = 1;
    while (1) {
        if (flow->process()) {
            if (count--) {
                // Ignore the signal
                flow->add_signal(SIGINT, []{}); 
            } else {
                // Then Ctrl+c will interrupt the process.
                flow->del_signal(SIGINT);   
            } 
        } else {
            sleep(5);
        }
    }
}