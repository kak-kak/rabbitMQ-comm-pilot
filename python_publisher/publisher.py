import pika
import time

# 接続の再試行ロジック
max_retries = 5
for i in range(max_retries):
    try:
        connection = pika.BlockingConnection(pika.ConnectionParameters('rabbitmq'))
        break
    except Exception as e:
        print(f"Connection attempt {i+1}/{max_retries} failed: {e}")
        time.sleep(5)  # 5秒待機
else:
    print("Failed to connect to RabbitMQ after several attempts.")
    exit(1)

channel = connection.channel()

# ファンアウト型の交換所を宣言
exchange_name = 'logs2'
channel.exchange_declare(exchange=exchange_name, exchange_type='fanout', durable=True)

# メッセージを送信
for i in range(1000):
    message = f"Hello from Python {i}"
    channel.basic_publish(exchange=exchange_name, routing_key='', body=message)
    print(f"Message sent: {message}")
    time.sleep(1)

connection.close()
