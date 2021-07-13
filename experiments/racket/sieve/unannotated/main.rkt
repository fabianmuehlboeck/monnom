#lang racket/base

;; Use the partner file "streams.rkt" to implement the Sieve of Eratosthenes.
;; Then compute and print the 10,000th prime number.

(require "streams.rkt")

;;--------------------------------------------------------------------------------------------------

;; `count-from n` Build a stream of integers starting from `n` and iteratively adding 1
(define (count-from n)
  (make-stream n (lambda () (count-from (add1 n)))))

;; `sift n st` Filter all elements in `st` that are equal to `n`.
;; Return a new stream.
(define (sift n st)
  (let ([hd (stream-first st)])
  (cond [(= 0 (modulo hd n)) (sift n (stream-getrest st))]
        [else (make-stream hd (lambda () (sift n (stream-getrest st))))])))

;; `sieve st` Sieve of Eratosthenes
(define (sieve st)
  (let ([hd (stream-first st)])
  (make-stream hd (lambda () (sieve (sift hd (stream-getrest st)))))))

;; stream of prime numbers
(define primes (sieve (count-from 2)))

;; Compute the 10,000th prime number
(define N-1 9999)

(define (main)
  (printf "The ~a-th prime number is: ~a\n" (add1 N-1) (stream-get primes N-1)))

(time (main))
