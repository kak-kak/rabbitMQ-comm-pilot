package main

import (
	"log"
	"time"

	"github.com/streadway/amqp"
)

const (
	maxRetries     = 5
	retryInterval  = 5 * time.Second
	rabbitMQServer = "amqp://rabbitmq" //"amqp://guest:guest@rabbitmq:5672/"
	exchangeName   = "logs"
)

func main() {
	var conn *amqp.Connection
	var err error

	for i := 0; i < maxRetries; i++ {
		conn, err = amqp.Dial(rabbitMQServer)
		if err == nil {
			break
		}
		log.Printf("Connection attempt %d/%d failed: %v", i+1, maxRetries, err)
		time.Sleep(retryInterval)
	}

	if err != nil {
		log.Fatalf("Failed to connect to RabbitMQ after several attempts: %v", err)
	}
	defer conn.Close()

	ch, err := conn.Channel()
	if err != nil {
		log.Fatalf("Failed to open a channel: %v", err)
	}
	defer ch.Close()

	err = ch.ExchangeDeclare(
		exchangeName, // name
		"fanout",     // type
		false,        // durable
		false,        // auto-deleted
		false,        // internal
		false,        // no-wait
		nil,          // arguments
	)
	if err != nil {
		log.Fatalf("Failed to declare an exchange: %v", err)
	}

	q, err := ch.QueueDeclare(
		"",    // name
		false, // durable
		false, // delete when unused
		true,  // exclusive
		false, // no-wait
		nil,   // arguments
	)
	if err != nil {
		log.Fatalf("Failed to declare a queue: %v", err)
	}

	err = ch.QueueBind(
		q.Name,       // queue name
		"",           // routing key
		exchangeName, // exchange
		false,
		nil,
	)
	if err != nil {
		log.Fatalf("Failed to bind a queue: %v", err)
	}

	msgs, err := ch.Consume(
		q.Name, // queue
		"",     // consumer
		true,   // auto-ack
		false,  // exclusive
		false,  // no-local
		false,  // no-wait
		nil,    // args
	)
	if err != nil {
		log.Fatalf("Failed to register a consumer: %v", err)
	}

	forever := make(chan bool)

	go func() {
		for d := range msgs {
			log.Printf("Received message: %s", d.Body)
		}
	}()

	log.Printf("Waiting for messages. To exit press CTRL+C")
	<-forever
}
