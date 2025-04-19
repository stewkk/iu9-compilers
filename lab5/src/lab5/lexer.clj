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
(def transitions '[])

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
