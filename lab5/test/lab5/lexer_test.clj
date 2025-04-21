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
    (is (= got (list (list (->Token :INT "123"))
                     '())))))

(deftest get-character-class-test
  (let [got (get-character-classes \a)]
    (is (= got (list 1 2 6 8 10 12 14 16 21)))))

(deftest tokenize-internal-simple-text-test
  (let [got (tokenize-internal "123 a1 def return1: (oaoa)")]
    (is (= got (list (reverse (list (->Token :INT "123")
                           (->Token :WS " ")
                           (->Token :IDENT "a1")
                           (->Token :WS " ")
                           (->Token :DEF "def")
                           (->Token :WS " ")
                           (->Token :IDENT "return1")
                           (->Token :COLON ":")
                           (->Token :WS " ")
                           (->Token :LPAREN "(")
                           (->Token :IDENT "oaoa")
                           (->Token :RPAREN ")")))
                     '())))))

(deftest tokenize-simple-text-test
  (let [got (tokenize "123 a1 def return1: (oaoa)")]
    (is (= got (list (list (->Token :INT "123")
                           (->Token :IDENT "a1")
                           (->Token :DEF "def")
                           (->Token :IDENT "return1")
                           (->Token :COLON ":")
                           (->Token :LPAREN "(")
                           (->Token :IDENT "oaoa")
                           (->Token :RPAREN ")")
                           (->Token :EOF ""))
                     '())))))
