(ns lab5.lexer
  (:gen-class)
  (:require [clojure.string :as str]
            [lab5.models.token :refer :all]))

(defn -main
  "I don't do a whole lot ... yet."
  [& _]
  (println "Hello, World!"))

(defn alnum-exclude-character [character]
  #(and (not= character %)
        (Character/isLetterOrDigit %)))

(def character-classes [(list #(str/includes? "\t\n " (str %))
                    '(0 1)
                    '(1 1))
                  (list #(and (Character/isLetter %)
                          (not (str/includes? "rd" (str %))))
                    '(0 2))
                  (list #(Character/isLetterOrDigit %)
                    '(2 2)
                    '(12 2)
                    '(6 2))
                  (list #(Character/isDigit %)
                    '(0 3)
                    '(3 3))
                  (list #(= \d %)
                    '(0 4))
                  (list #(= \e %)
                    '(4 5))
                  (list (alnum-exclude-character \e)
                    '(4 2)
                    '(7 2))
                  (list #(= \f %)
                    '(5 6))
                  (list (alnum-exclude-character \f)
                    '(5 2))
                  (list #(= \t %)
                    '(8 9))
                  (list (alnum-exclude-character \t)
                    '(8 2))
                  (list #(= \u %)
                    '(9 10))
                  (list (alnum-exclude-character \u)
                    '(9 2))
                  (list #(= \r %)
                    '(10 11))
                  (list (alnum-exclude-character \r)
                    '(10 2))
                  (list #(= \n %)
                    '(11 12))
                  (list (alnum-exclude-character \n)
                    '(11 2))
                  (list #(= \( %)
                    '(0 13))
                  (list #(= \) %)
                    '(0 14))
                  (list #(= \: %)
                    '(0 15))
                  (list #(= \" %)
                    '(0 16)
                    '(16 17)
                    '(17 18)
                    '(18 20)
                    '(19 20)
                    '(20 21)
                    '(21 22))
                  (list #(not= \" %)
                    '(18 19)
                    '(19 19)
                    '(20 19)
                    '(21 19))])


(defn get-character-classes [character]
  (->> (map-indexed (fn [index class]
                      (if ((first class) character)
                        index
                        nil))
                    character-classes)
       (filter #(not (nil? %)))))

(def transitions (loop [transitions (into [] (repeat 22 (into [] (repeat (count character-classes) -1))))
                        classes (seq character-classes)
                        i 0]
                   (if (empty? classes)
                     transitions
                     (recur (loop [transitions transitions
                                   edges (rest (first classes))]
                              (if (empty? edges)
                                transitions
                                (recur (assoc-in transitions
                                                 [(first (first edges)) i]
                                                 (first (rest (first edges))))
                                       (rest edges))))
                            (rest classes)
                            (+ i 1)))))

(defn make-transition [state character]
  (let [state-transitions (get transitions state)
        transition-list (->> (get-character-classes character)
                             (map (fn [index] (get state-transitions index)))
                             (filter #(not= % -1)))]
    (if (empty? transition-list)
      nil
      (first transition-list))))

(defn tokenize
  [text]
  (loop [state 0
         symbols (seq text)
         tokens '()
         messages '()]
    (if (empty? symbols)
      (list tokens messages)
      (let [new-state (make-transition state (first symbols))]
        (if (nil? new-state)
          nil ; TODO: return last final state
          (recur new-state
           (rest symbols)
           tokens
           messages))))))
