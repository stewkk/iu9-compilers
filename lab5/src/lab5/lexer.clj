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
                              '(7 8)
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
                              '(0 7)
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

(def state-to-lexem-class {1 :WS
                           2 :IDENT
                           3 :INT
                           4 :IDENT
                           5 :IDENT
                           6 :DEF
                           7 :IDENT
                           8 :IDENT
                           9 :IDENT
                           10 :IDENT
                           11 :IDENT
                           12 :RETURN
                           13 :LPAREN
                           14 :RPAREN
                           15 :COLON
                           22 :COMMENT
                           })

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

(defn is-final? [state]
  (not (nil? (get state-to-lexem-class state))))

(defn spymy
  "Print + Return"
  [x]
  (prn x)
  x)

(defn coords [start end]
  (->Coordinates (->Position (first start) (first (rest start)))
                 (->Position (first end) (first (rest end)))))

(defn get-token [state image start end]
  (->Token (state-to-lexem-class state)
           (str/join (reverse image))
           (->Coordinates start end)))

(defn next-position [pos character]
  (if (= \newline character)
    (->Position (+ 1 (get pos :line))
                (get pos :column))
    (->Position (get pos :line)
                (+ 1 (get pos :column)))))

(defn tokenize-internal
  [text]
  (loop [state 0
         final nil
         image '()
         characters (seq text)
         start (->Position 1 1)
         end nil
         current (->Position 1 1)
         tokens '()
         messages '()]
    (if (empty? characters)
      (if (nil? final)
        (list tokens messages current)
        (list (cons (get-token final image start end) tokens)
              messages
              current))
      (let [character (first characters)
            new-state (make-transition state character)]
        (condp = (list new-state final)
          (list nil nil) (recur state
                                final
                                image
                                (rest characters)
                                start
                                end
                                (next-position current character)
                                tokens
                                (cons (format "error at %c" character) messages))
          (list nil final) (recur 0
                                  nil
                                  '()
                                  characters
                                  current
                                  end
                                  current
                                  (cons (get-token final image start end) tokens)
                                  messages)
          (list new-state final) (if (is-final? new-state)
                                   (recur new-state
                                          new-state
                                          (cons character image)
                                          (rest characters)
                                          start
                                          current
                                          (next-position current character)
                                          tokens
                                          messages)
                                   (recur new-state
                                          final
                                          (cons character image)
                                          (rest characters)
                                          start
                                          end
                                          (next-position current character)
                                          tokens
                                          messages)))))))

(defn tokenize [text]
  (let [tmp (tokenize-internal text)
        tokens (first tmp)
        messages (first (rest tmp))
        end (first (rest (rest tmp)))]
    (list (reverse
           (cons (->Token :EOF "" (->Coordinates end end))
                 (filter #(not= (get % :class) :WS)
                         tokens)))
          messages)))
