(ns lab5.lexer-test
  (:require [clojure.test :refer :all]
            [lab5.lexer :refer :all]
            [lab5.models.token :refer :all]))

(defn spy
  "Print + Return"
  [x]
  (prn x)
  x)

(deftest tokenize-test
  (let [got (tokenize-internal "123")]
    (is (= got (list (list (->Token :INT "123" (coords '(1 1) '(1 3))))
                     '()
                     (->Position 1 4))))))

(deftest get-character-class-test
  (let [got (get-character-classes \a)]
    (is (= got (list 1 2 6 8 10 12 14 16 21)))))

(deftest tokenize-internal-simple-text-test
  (let [got (tokenize-internal "123 a1 def return1: (oaoa)")]
    (is (= got (list (reverse (list (->Token :INT "123" (coords '(1 1) '(1 3)))
                                    (->Token :WS " " (coords '(1 4) '(1 4)))
                                    (->Token :IDENT "a1" (coords '(1 5) '(1 6)))
                                    (->Token :WS " " (coords '(1 7) '(1 7)))
                                    (->Token :DEF "def" (coords '(1 8) '(1 10)))
                                    (->Token :WS " " (coords '(1 11) '(1 11)))
                                    (->Token :IDENT "return1" (coords '(1 12) '(1 18)))
                                    (->Token :COLON ":" (coords '(1 19) '(1 19)))
                                    (->Token :WS " " (coords '(1 20) '(1 20)))
                                    (->Token :LPAREN "(" (coords '(1 21) '(1 21)))
                                    (->Token :IDENT "oaoa" (coords '(1 22) '(1 25)))
                                    (->Token :RPAREN ")" (coords '(1 26) '(1 26)))))
                     '()
                     (->Position 1 27))))))

(deftest tokenize-simple-text-test
  (let [got (tokenize "123 a1 def return1: (oaoa)")]
    (is (= got (list (list (->Token :INT "123" (coords '(1 1) '(1 3)))
                           (->Token :IDENT "a1" (coords '(1 5) '(1 6)))
                           (->Token :DEF "def" (coords '(1 8) '(1 10)))
                           (->Token :IDENT "return1" (coords '(1 12) '(1 18)))
                           (->Token :COLON ":" (coords '(1 19) '(1 19)))
                           (->Token :LPAREN "(" (coords '(1 21) '(1 21)))
                           (->Token :IDENT "oaoa" (coords '(1 22) '(1 25)))
                           (->Token :RPAREN ")" (coords '(1 26) '(1 26)))
                           (->Token :EOF "" (coords '(1 27) '(1 27))))
                     '())))))
