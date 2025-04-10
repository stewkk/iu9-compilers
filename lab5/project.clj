(defproject lab5 "0.1.0-SNAPSHOT"
  :description "lexical analyzer writeen in clojure"
  :url "https://github.com/stewkk/iu9-compilers/tree/master/lab5"
  :license {:name "unlicensed"
            :url ""}
  :dependencies [[org.clojure/clojure "1.11.1"]]
  :main ^:skip-aot lab5.lexer
  :target-path "target/%s"
  :profiles {:uberjar {:aot :all
                       :jvm-opts ["-Dclojure.compiler.direct-linking=true"]}})
