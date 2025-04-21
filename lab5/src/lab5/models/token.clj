(ns lab5.models.token)

(defrecord Token [class image coords])

(defrecord Coordinates [start end])

(defrecord Position [line column])
