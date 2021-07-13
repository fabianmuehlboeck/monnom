#lang typed/racket/base

;; Simple streams library.
;; For building and using infinite lists.

(provide (struct-out stream)
         make-stream
         stream-first
         stream-getrest
         stream-get
         #;stream-take)

;; ;; A stream is a cons of a value and a thunk that computes the next value when applied
(struct: stream ([first : Natural] [rest : (-> stream)]))

;;--------------------------------------------------------------------------------------------------

(: make-stream (-> Natural (-> stream) stream))
(define (make-stream hd thunk)
  (stream hd thunk))

(: stream-getrest (-> stream stream))
(define (stream-getrest st)
  ((stream-rest st)))

;; [stream-get st i] Get the [i]-th element from the stream [st]
(: stream-get (-> stream Natural Natural))
(define (stream-get st i)
  #;(define-values (hd tl) (stream-unfold st))
  (cond [(= i 0) (stream-first st)]
        [else    (stream-get (stream-getrest st) (sub1 i))]))

;; [stream-take st n] Collect the first [n] elements of the stream [st].
#;(: stream-take (-> stream Natural (Listof Natural)))
#;(define (stream-take st n)
  (cond [(= n 0) '()]
        [else (define-values (hd tl) (stream-unfold st))
              (cons hd (stream-take tl (sub1 n)))]))
