```mermaid
graph TD;
    A["Main Control Unit (MCU)"] --> B["Sensor Manager"];
    A --> C["Actuator Control"];
    A --> D["Communication Handler"];
    A --> E["Safety Monitoring"];
    A --> F["Power Management"];

    B --> B1["Read Sensors"];
    B --> B2["Filter Data"];
    B --> B3["Send Data to MCU"];

    C --> C1["Throttle Control"];
    C --> C2["Brake Control"];
    C --> C3["Steering Control"];

    D --> D1["CAN Bus Handler"];
    D --> D2["Serial Communication"];
    D --> D3["Wireless Communication"];

    E --> E1["Watchdog Timer"];
    E --> E2["Error Handling"];
    E --> E3["Fault Recovery"];

    F --> F1["Battery Management"];
    F --> F2["Voltage Regulation"];
    F --> F3["Power Distribution"];
