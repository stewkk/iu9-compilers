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

(def character-classes [#(str/includes? "\t\n " (str %))
                        #(and (Character/isLetter %)
                              (not (str/includes? "rd" (str %))))
                        #(Character/isLetterOrDigit %)
                        #(Character/isDigit %)
                        #(= \d %)
                        #(= \e %)
                        (alnum-exclude-character \e)
                        #(= \f %)
                        (alnum-exclude-character \f)
                        #(= \t %)
                        (alnum-exclude-character \t)
                        #(= \u %)
                        (alnum-exclude-character \u)
                        #(= \r %)
                        (alnum-exclude-character \r)
                        #(= \n %)
                        (alnum-exclude-character \n)
                        #(= \( %)
                        #(= \) %)
                        #(= \: %)
                        #(= \" %)
                        #(not= \" %)])

(defn get-character-classes [character]
  (->> (map-indexed (fn [index pred]
                      (if (pred character)
                        index
                        -1))
                    character-classes)
       (filter #(not= % -1))))

(defn generate-transitions [& edges]
  (let [res (into [] (repeat 22 -1))]
    (loop [edges edges
           res res]
      (if (empty? edges)
        res
        (let [edge (first edges)]
          (recur (rest edges)
                  (assoc res
                         (first edge)
                         (first (rest edge)))))))))

;; [state][character class]
(def transitions '[(generate-transitions (0 1)
                                         (1 2)
                                         (3 3)
                                         (4 4)
                                         (13 7)
                                         (17 13)
                                         (18 14)
                                         (19 15)
                                         (20 16))
                   (generate-transitions (0 1))
                   (generate-transitions (2 2))
                   (generate-transitions (3 3))
                   (generate-transitions (5 5) (6 2))
                   (generate-transitions (7 6) (8 2))
                   (generate-transitions (2 2))
                   (generate-transitions (5 8) (6 2))
                   (generate-transitions (9 9) (10 2))
                   (generate-transitions (11 10) (12 2))
                   (generate-transitions (13 11) (14 2))
                   (generate-transitions (15 12) (16 2))
                   (generate-transitions (2 2))
                   (generate-transitions (20 17))
                   (generate-transitions (20 18))
                   (generate-transitions (20 20) (21 19))
                   (generate-transitions (21 19) (20 20))
                   (generate-transitions (21 19) (20 21))
                   (generate-transitions (21 19) (20 22))
                   ])

(defn make-transition [state character]
  (let [state-transitions (get transitions state)]
    (->> (get-character-classes character)
         (map (fn [index] (get state-transitions index)))
         first)))

(defn tokenize
  [text]
  (loop [state 0
         symbols (seq text)
         tokens '()
         messages '()]
    (if (empty? symbols)
      (list tokens messages)
      (recur (make-transition state (first symbols))
             (rest symbols)
             tokens
             messages))))
