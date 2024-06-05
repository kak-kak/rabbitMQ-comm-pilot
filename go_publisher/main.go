package main

import (
	"fmt"
	"log"
	"time"

	"github.com/streadway/amqp"
)

func main() {
	const maxRetries = 5
	const retryInterval = 5 * time.Second
	exchangeName := "logs2"
	amqpURL := "amqp://rabbitmq"

	var conn *amqp.Connection
	var err error

	for i := 0; i < maxRetries; i++ {
		conn, err = amqp.Dial(amqpURL)
		if err == nil {
			break
		}
		log.Printf("Connection attempt %d/%d failed: %v", i+1, maxRetries, err)
		time.Sleep(retryInterval)
	}

	if err != nil {
		log.Fatalf("Failed to connect to RabbitMQ after %d attempts: %v", maxRetries, err)
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
		true,         // durable
		false,        // auto-deleted
		false,        // internal
		false,        // no-wait
		nil,          // arguments
	)
	if err != nil {
		log.Fatalf("Failed to declare an exchange: %v", err)
	}

	for i := 0; i < 1000; i++ {
		body := fmt.Sprintf("Hello from Go %d", i)
		err = ch.Publish(
			exchangeName, // exchange
			"",           // routing key
			false,        // mandatory
			false,        // immediate
			amqp.Publishing{
				ContentType: "text/plain",
				Body:        []byte(body),
			})
		if err != nil {
			log.Fatalf("Failed to publish a message: %v", err)
		}
		fmt.Printf("Message sent: %s\n", body)
		time.Sleep(1 * time.Second)
	}
}
