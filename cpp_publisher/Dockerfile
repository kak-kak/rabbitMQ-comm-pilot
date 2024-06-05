FROM ubuntu:22.04

# 必要なライブラリをインストール
RUN apt-get update && \
    apt-get install -y cmake libboost-all-dev libssl-dev

RUN apt-get update && \
    apt-get -y install librabbitmq-dev build-essential git

# SimpleAmqpClientをインストール
RUN git clone https://github.com/alanxz/SimpleAmqpClient.git /SimpleAmqpClient && \
    cd /SimpleAmqpClient && \
    cmake . && \
    make && \
    make install

RUN ln -s /SimpleAmqpClient/build/libSimpleAmqpClient.so.7 /usr/lib/libSimpleAmqpClient.so.7

# C++のソースコードをコピー
COPY main.cpp /usr/src/main.cpp

# コンパイルとリンカの設定
RUN g++ /usr/src/main.cpp -o /usr/bin/cpp_publisher -L/usr/local/lib -lSimpleAmqpClient -lboost_system -lpthread

# 動的リンカーのキャッシュを更新
RUN ldconfig

# 環境変数の設定
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

RUN apt-get update && \
    apt-get -y install iputils-ping

# 実行コマンド
CMD ["sh", "-c", "sleep 5 && ping -c 10 rabbitmq && cpp_publisher"]