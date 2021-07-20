#lang racket

(provide run)

(define (filter-line line)
  (and
    (not (= (string-length line) 0))
    (not (string=? line "Dynamic : ?"))
    (not (string=? line "Unit : ()"))
    (not (string=? (substring line 0 5) "FILE:"))
    (not (string=? (substring line 1 3) ":\\")) ))

(define (process-line line)
  (if (and (> (string-length line) 8) 
           (string=? (substring line (- (string-length line) 8)) " Seconds"))
      (substring line 0 (- (string-length line) 8))
      (if (and (> (string-length line) 11)
               (string=? (substring line 0 11) "time (sec):"))
          (substring line 12)
          line)))

(define (process-folder folder)
  (let-values ([(progpath progname progisdir) (split-path folder)])
    (string-join
     (cons (path->string progname)
           (append* (map
		(lambda (file)
		  (map process-line
		   (filter
		      filter-line
		      (file->lines (build-path folder file)))))
		(get-outfiles folder))))
           ",")))

(define (get-directories dir)
  (map path->complete-path (filter (lambda (x) (string=? (substring (let-values ([(base name mbd) (split-path x)]) (path->string name)) 0 4) ".BM_")) (directory-list dir))))

(define (get-outfiles dir)
  (map path->string (filter (lambda (x) (let-values ([(base name mbd) (split-path x)]) (and (string=? (substring (path->string name) 0 3) "out") (string=? (substring (path->string name) (- (string-length (path->string name)) 4)) ".txt")))) (directory-list dir))))

(define (run udir)
  (let ([dir (expand-user-path udir)])
	  (current-directory dir)
	  (let* ([directories (get-directories dir)]
		 [lines (map process-folder directories)]
		 [outpath (build-path dir "results.csv")])
	    (if (file-exists? outpath)
	       (delete-file outpath)
	       '())
	    (display-lines-to-file lines outpath))))
