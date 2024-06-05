FROM python:3.9-slim

# pikaをインストール
RUN pip install pika

# Pythonのソースコードをコピー
COPY publisher.py /usr/src/publisher.py

# 実行コマンド
CMD ["sh", "-c", "sleep 5 && python /usr/src/publisher.py"]