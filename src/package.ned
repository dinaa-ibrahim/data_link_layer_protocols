package data_link_layer_protocols_simulation;

@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=520,344");
    submodules:
        coordinator: Coordinator {
            @display("p=261,143");
        }
        node0: Node {
            @display("p=214,220");
        }
        node1: Node {
            @display("p=318,220");
        }
        node3: Node {
            @display("p=175,44");
        }
        node2: Node {
            @display("p=64,44");
        }
        node4: Node {
            @display("p=446,44");
        }
        node5: Node {
            @display("p=346,48");
        }
    connections:
        coordinator.out[0] --> node0.inCoordinator;
        coordinator.out[1] --> node1.inCoordinator;
        node0.out --> node1.in;
        node1.out --> node0.in;
        node3.out --> node2.in;
        node2.out --> node3.in;
        coordinator.out[4] --> node4.inCoordinator;
        coordinator.out[3] --> node3.inCoordinator;
        coordinator.out[2] --> node2.inCoordinator;
        coordinator.out[5] --> node5.inCoordinator;
        node5.out --> node4.in;
        node4.out --> node5.in;
}

