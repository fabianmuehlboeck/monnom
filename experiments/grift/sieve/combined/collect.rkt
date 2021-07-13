#lang racket

(define (process-folder folder)
  (let ([progname (last (string-split folder "/"))])
    (string-join
     (cons progname
           (append* (map
                (lambda (file)
                  (map (lambda (v) (if (and (> (string-length v) 8) (string=? (substring v (- (string-length v) 8)) " Seconds"))
                                       (substring v 0 (- (string-length v) 8))
                                       v))
           (filter
            (lambda (x)
              (and
               (not (= (string-length x) 0))
               #;(not (string=? (substring x 0 5) "FILE:"))
               #;(not (string=? (substring x 1 3) ":\\"))))
              (string-split (file->string (string-append folder "/" file)) "\n"))))
                 (get-outfiles folder))))
           ",")))

(define (get-directories dir)
  (map path->string (map path->complete-path (filter (lambda (x) (string=? (substring (let-values ([(base name mbd) (split-path x)]) (path->string name)) 0 4) ".BM_")) (directory-list dir)))))

(define (get-outfiles dir)
  (map path->string (filter (lambda (x) (let-values ([(base name mbd) (split-path x)]) (and (string=? (substring (path->string name) 0 3) "out") (string=? (substring (path->string name) (- (string-length (path->string name)) 4)) ".txt")))) (directory-list dir))))

(define (run dir)
  (let* ([dirstr (path->string (path->complete-path dir))]
         [directories (get-directories dir)]
         [lines (map process-folder directories)]
         [outpath (string-append dirstr "/results.csv")])
    (if (file-exists? outpath)
       (delete-file outpath)
       '())
    (display-lines-to-file lines outpath)))

(run ".")
