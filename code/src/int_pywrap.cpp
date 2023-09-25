// int_pywrap.cpp
#include <pybind11/pybind11.h>
#include "Queue.h"
#include "Queue.cpp"

namespace py = pybind11;

PYBIND11_MODULE(Queue_cpp_int, m) {
    m.doc() = "Documentazione";

    m.def("threads", &QUEUE_H::threads);
    m.def("best_effort", &QUEUE_H::best_effort);
    m.def("reliability", &QUEUE_H::reliability);
    m.def("dim", &QUEUE_H::dim);
    m.def("int_to_sizet", &QUEUE_H::int_to_sizet);

    py::class_<Queue<int>>(m, "Queue")
        .def(py::init<bool, int, size_t>(), py::arg("gest")=RELIABILITY, py::arg("levels")=3, py::arg("dim")=DIM) //Rendo accesibile il primo costruttore
        //Rendo visibili i getters
        .def("getLevels", &Queue<int>::getLevels)
        .def("getDim", &Queue<int>::getDim)
        .def("getQoS", &Queue<int>::isReliability)
        .def("isEmpty", &Queue<int>::isEmpty)
        .def("show", &Queue<int>::show)
        .def("pop", &Queue<int>::pop, py::call_guard<py::gil_scoped_release>()) //Rendo accessibile ovviamente la pop
        .def("push", &Queue<int>::push, py::call_guard<py::gil_scoped_release>(), py::arg("element")=1, py::arg("priority")=0); //Rendo accessibile ovviamente la push
}