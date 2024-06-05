#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept> // std::runtime_error を使用するために必要

int main()
{
    const int MAX_RETRIES = 5;
    const std::chrono::seconds RETRY_INTERVAL(5);
    const std::string AMQP_URL = "rabbitmq";
    const std::string EXCHANGE_NAME = "logs";

    AmqpClient::Channel::ptr_t channel;
    int retryCount = 0;
    int maxRetries = 5;
    std::chrono::seconds retryDelay(1);

    while (retryCount < maxRetries)
    {
        try
        {
            channel = AmqpClient::Channel::Create(AMQP_URL);
            if (!channel)
            {
                std::cerr << "Failed to connect after " << maxRetries << " retries." << std::endl;
            }
            break; // 接続成功
        }
        catch (const AmqpClient::AmqpLibraryException &e)
        {
            std::cerr << "Connection failed: " << e.what() << std::endl;
            retryCount++;
            std::this_thread::sleep_for(retryDelay);
            retryDelay *= 2; // 指数バックオフ
        }
    }

    if (!channel)
    {
        std::cerr << "Failed to connect after " << maxRetries << " retries." << std::endl;
        return 1;
        // エラー処理
    }

    // ファンアウト型の交換所を宣言
    channel->DeclareExchange(EXCHANGE_NAME, AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);

    // メッセージを送信
    for (int i = 0; i < 1000; ++i)
    {
        std::string message = "Hello from C++ " + std::to_string(i);
        channel->BasicPublish(EXCHANGE_NAME, "", AmqpClient::BasicMessage::Create(message));
        std::cout << "Message sent: " << message << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
