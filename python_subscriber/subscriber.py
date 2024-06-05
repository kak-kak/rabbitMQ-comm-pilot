import pika
import time

# 接続の再試行ロジック
max_retries = 5
for i in range(max_retries):
    try:
        # connection_params = pika.ConnectionParameters('rabbitmq', 5672, '/', pika.PlainCredentials('guest', 'guest'))
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
exchange_name = 'logs'
channel.exchange_declare(exchange=exchange_name, exchange_type='fanout')

# 一時的なキューを宣言（RabbitMQが自動生成するキュー）
result = channel.queue_declare(queue='', exclusive=True)
queue_name = result.method.queue

# キューをファンアウト型の交換所にバインド
channel.queue_bind(exchange=exchange_name, queue=queue_name)

print('Waiting for messages. To exit press CTRL+C')

def callback(ch, method, properties, body):
    print(f"Received message: {body.decode()}")

channel.basic_consume(queue=queue_name, on_message_callback=callback, auto_ack=True)
channel.start_consuming()
