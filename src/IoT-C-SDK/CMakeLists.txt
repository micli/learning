cmake_minimum_required(VERSION 3.2)

set(AZUREIOT_INC_FOLDER ".." "/usr/include/azureiot" "/usr/include/azureiot/inc")

include_directories(${AZUREIOT_INC_FOLDER})

set(sample_application_c_files
    ./IoTSimpleSample.c
    ./main.c
)

set(sample_application_h_files
    ./IoTSimpleSample.h
)

add_executable(IoTDevice_sample_app ${sample_application_c_files} ${sample_application_h_files})

target_link_libraries(IoTDevice_sample_app
    serializer
    iothub_client
    iothub_client_amqp_transport
    aziotsharedutil
    uamqp
    pthread
    curl
    ssl
    crypto
    m
)
