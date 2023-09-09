// pywrap.cpp
#include <pybind11/pybind11.h>
#include "Queue.h"
#include "Queue.cpp"

namespace py = pybind11;
constexpr auto byref = py::return_value_policy::reference_internal;

PYBIND11_MODULE(Queue_cpp, m) {
    m.doc() = "Documentazione";

    m.def("threads", &QUEUE_H::threads);
    m.def("fifo", &QUEUE_H::fifo);
    m.def("fixed_priority", &QUEUE_H::fixed_priority);
    m.def("best_effort", &QUEUE_H::reliability);
    m.def("reliability", &QUEUE_H::reliability);
    m.def("dim", &QUEUE_H::dim);
    m.def("int_to_sizet", &QUEUE_H::int_to_sizet);

    py::class_<Queue<int>>(m, "Queue")
        .def(py::init<int, int, int, size_t>()) //Rendo accesibile il primo costruttore
        .def(py::init<size_t, int>()) //Rendo accessible il secondo costruttore
        .def("pop", &Queue<int>::pop, py::call_guard<py::gil_scoped_release>()) //Rendo accessibile ovviamente la pop
        .def("push", &Queue<int>::push, py::call_guard<py::gil_scoped_release>()); //Rendo accessibile ovviamente la push
}