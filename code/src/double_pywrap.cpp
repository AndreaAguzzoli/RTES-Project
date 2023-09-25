// int_pywrap.cpp
#include <pybind11/pybind11.h>
#include "Queue.h"
#include "Queue.cpp"

namespace py = pybind11;

PYBIND11_MODULE(Queue_cpp_double, m) {
    m.doc() = "Documentazione";

    m.def("threads", &QUEUE_H::threads);
    m.def("best_effort", &QUEUE_H::best_effort);
    m.def("reliability", &QUEUE_H::reliability);
    m.def("dim", &QUEUE_H::dim);
    m.def("int_to_sizet", &QUEUE_H::int_to_sizet);

    py::class_<Queue<double>>(m, "Queue")
        .def(py::init<bool, int, size_t>(), py::arg("gest")=RELIABILITY, py::arg("levels")=3, py::arg("dim")=DIM) //Rendo accesibile il primo costruttore
        //Rendo visibili i getters
        .def("getLevels", &Queue<double>::getLevels)
        .def("getDim", &Queue<double>::getDim)
        .def("getQoS", &Queue<double>::isReliability)
        .def("isEmpty", &Queue<double>::isEmpty)
        .def("show", &Queue<double>::show)
        .def("pop", &Queue<double>::pop, py::call_guard<py::gil_scoped_release>()) //Rendo accessibile ovviamente la pop
        .def("push", &Queue<double>::push, py::call_guard<py::gil_scoped_release>(), py::arg("element")=-1.0, py::arg("priority")=0); //Rendo accessibile ovviamente la push
}