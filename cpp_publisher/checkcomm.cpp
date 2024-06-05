#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <cstdlib>  // for getenv
#include <unistd.h> // for sleep

int main()
{
    const char *rabbitmq_host = std::getenv("RABBITMQ_HOST");
    if (rabbitmq_host == nullptr)
    {
        std::cerr << "RABBITMQ_HOST environment variable is not set" << std::endl;
        return 1;
    }
    std::string amqp_url = std::string("amqp://guest:guest@") + rabbitmq_host + ":5672/";

    // Retry logic for connection
    int retries = 5;
    while (retries--)
    {
        try
        {
            auto channel = AmqpClient::Channel::Create("rabbitmq");
            std::cout << "Connected to RabbitMQ" << std::endl;
            break; // Exit loop if connection is successful
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to connect to RabbitMQ: " << e.what() << std::endl;
            std::cerr << "Retries left: " << retries << std::endl;
            sleep(5); // Wait before retrying
        }
    }

    if (retries == 0)
    {
        std::cerr << "Failed to connect after multiple retries." << std::endl;
        return 1;
    }

    return 0;
}
