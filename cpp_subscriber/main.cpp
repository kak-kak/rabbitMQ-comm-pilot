#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>

int main()
{
    const int MAX_RETRIES = 5;
    const std::chrono::seconds RETRY_INTERVAL(5);
    const std::string AMQP_URL = "rabbitmq";
    const std::string EXCHANGE_NAME = "logs2";
    const int TIMEOUT_MS = 5000; // タイムアウトをミリ秒で指定

    AmqpClient::Channel::ptr_t channel;
    int retryCount = 0;

    while (retryCount < MAX_RETRIES)
    {
        try
        {
            channel = AmqpClient::Channel::Create(AMQP_URL);
            if (!channel)
            {
                std::cerr << "Failed to connect after " << MAX_RETRIES << " retries." << std::endl;
            }
            break; // 接続成功
        }
        catch (const AmqpClient::AmqpLibraryException &e)
        {
            std::cerr << "Connection failed: " << e.what() << std::endl;
            retryCount++;
            std::this_thread::sleep_for(RETRY_INTERVAL);
        }
    }

    if (!channel)
    {
        std::cerr << "Failed to connect after " << MAX_RETRIES << " retries." << std::endl;
        return 1;
    }

    // ファンアウト型の交換所を宣言
    channel->DeclareExchange(EXCHANGE_NAME, AmqpClient::Channel::EXCHANGE_TYPE_FANOUT, true, true, false);

    // 一時的なキューを宣言
    std::string queue_name = channel->DeclareQueue("", false, true, true, true);
    channel->BindQueue(queue_name, EXCHANGE_NAME, "");

    std::cout << "Waiting for messages. To exit press CTRL+C" << std::endl;

    // メッセージの消費を開始
    auto consumer_tag = channel->BasicConsume(queue_name, "", true, false, false, 1);

    // 無限ループでメッセージを受信
    while (true)
    {
        AmqpClient::Envelope::ptr_t envelope;
        bool received = channel->BasicConsumeMessage(consumer_tag, envelope, TIMEOUT_MS);

        if (received && envelope)
        {
            std::string message = envelope->Message()->Body();
            std::cout << "Received message: " << message << std::endl;
            channel->BasicAck(envelope);
        }
        else
        {
            std::cout << "No message received within timeout period." << std::endl;
        }
    }

    return 0;
}
