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
  (let [got (tokenize "123")]
    (is (= got (list (list (->Token :INT))
                     '())))))

(deftest get-character-class-test
  (let [got (get-character-classes \a)]
    (is (= got (list 1 2 6 8 10 12 14 16 21)))))

(deftest tokenize-simple-text-test
  (let [got (tokenize "123 a1 def return1: (oaoa)")]
    (is (= got (list (list (->Token :INT)
                           (->Token :WS)
                           (->Token :IDENT)
                           (->Token :WS)
                           (->Token :DEF)
                           (->Token :WS)
                           (->Token :IDENT)
                           (->Token :COLON)
                           (->Token :WS)
                           (->Token :LPAREN)
                           (->Token :IDENT)
                           (->Token :RPAREN))
                     '())))))
