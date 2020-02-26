#include "msq.h"

QualityImprover::QualityImprover(uint8_t c){
    cellnum = c;
    return;
}
QualityImprover::~QualityImprover(){}
uint8_t QualityImprover::ExecWrapper() {
    string filename = "./vtk/" + to_string(cellnum);
    filename += "_tmp.vtk";
    Mesquite::MsqError err;
    Mesquite::MeshImpl mymesh;
    mymesh.read_vtk(filename.c_str(), err);
    if (err) return 0;

    exec(mymesh, err);

    if (isTangled(mymesh, err))
        return 1;

    string outfilename = "./vtk/" + to_string(cellnum);
    outfilename += "_tmp_opt.vtk";
    mymesh.write_vtk(outfilename.c_str(), err);
    if (err) return 0;

    return 2;
}
uint8_t QualityImprover::isTangledWrapper() {
    string filename = "./vtk/" + to_string(cellnum);
    filename += "_tmp.vtk";
    Mesquite::MsqError err;
    Mesquite::MeshImpl mymesh;
    mymesh.read_vtk(filename.c_str(), err);
    if (err) return 0;

    return isTangled(mymesh, err);
}

uint8_t QualityImprover::CustomWrapper() {
    using namespace Mesquite;
    string filename = "./vtk/" + to_string(cellnum);
    filename += "_tmp.vtk";
    MeshImpl mesh;
    MsqError err;
    mesh.read_vtk(filename.c_str(), err);
    /* mesh.set_all_fixed_flags(0, err); */
    vector<Mesh::VertexHandle> vhs;
    vector<bool> fixedv;
    mesh.set_all_fixed_flags(false, err);
    UntangleBetaQualityMetric untangler;
    LPtoPTemplate obj_func(&untangler, 2, err);
    TrustRegion trust(&obj_func);
    trust.use_global_patch();
    TerminationCriterion e;
    e.add_relative_gradient_L2_norm(2e-2);
    trust.set_inner_termination_criterion(&e);
    InstructionQueue iqueue;
    iqueue.set_master_quality_improver(&trust, err);
    MeshDomainAssoc mesh_and_domain(&mesh, 0);
    iqueue.run_instructions(&mesh_and_domain, err);
    if (err) return 0;

    IdealShapeTarget target;
    TShapeSizeB3 mu_no;
    TQualityMetric metric_no_0(&target, &mu_no);
    ElementPMeanP metric_no(1.0, &metric_no_0);
    PMeanPTemplate obj_func_no(1.0, &metric_no);
    ConjugateGradient improver_no(&obj_func_no);;
    improver_no.use_global_patch();
    InstructionQueue queue;
    queue.set_master_quality_improver(&improver_no, err);
    queue.run_instructions(&mesh, err);
    if (err) return 0;
    string outfilename = "./vtk/" + to_string(cellnum);
    outfilename += "_tmp_opt.vtk";
    mesh.write_vtk(outfilename.c_str(), err);

    return 2;

}
uint8_t QualityImprover::exec(Mesquite::MeshImpl &mymesh, Mesquite::MsqError &err) {

    Mesquite::ShapeImprovementWrapper wrapper1;
    Mesquite::ShapeImprover wrapper;
    Mesquite::UntangleWrapper untangle;
    untangle.set_untangle_metric(Mesquite::UntangleWrapper::SIZE);

    streambuf* coutbuf = cout.rdbuf();
    ofstream of("tmp.dat");
    cout.rdbuf(of.rdbuf());
    //Mesquite::PlanarDomain plane(Mesquite::PlanarDomain::Plane(0));
    /* Mesquite::MeshDomainAssoc mesh_domain(&mymesh); */
    untangle.run_instructions(&mymesh, err);

    wrapper.run_instructions(&mymesh, err);
    wrapper1.run_instructions(&mymesh, err);
    cout.rdbuf(coutbuf);

    return 1;
}
uint8_t QualityImprover::isTangled(Mesquite::MeshImpl &mymesh, Mesquite::MsqError &err) {
    Mesquite::QualityAssessor qa(false, false);
    Mesquite::InstructionQueue q;
    q.add_quality_assessor(&qa, err);
    Mesquite::MeshDomainAssoc mesh_and_domain = Mesquite::MeshDomainAssoc(&mymesh, 0);

    streambuf* coutbuf = cout.rdbuf();
    ofstream of("tmp.dat");
    cout.rdbuf(of.rdbuf());
    q.run_common(&mesh_and_domain, 0, &q, err);
    int inverted_elems = 0, inverted_samples = 0;
    qa.get_inverted_element_count(inverted_elems, inverted_samples, err);
    cout.rdbuf(coutbuf);
    if (inverted_elems or inverted_samples) return 1;
    if (err) return 1;

    return 0;
}


bool Untangle(int cellnum) {
    string filename = "./vtk/" + to_string(cellnum);
    filename += "_tmp.vtk";
    Mesquite::MsqError err;
    Mesquite::MeshImpl mymesh;
    mymesh.read_vtk(filename.c_str(), err);

    Mesquite::UntangleWrapper untangle;
    streambuf *backup;
    ofstream fout;
    fout.open("tmp.dat");
    backup = cout.rdbuf();
    cout.rdbuf(fout.rdbuf());
    untangle.run_instructions(&mymesh, err);

    Mesquite::QualityAssessor check_inverted(false, false);
    Mesquite::InstructionQueue q_invert_check;
    // a QuallityAssessor without a metric will just check for inverted elements and samples
    q_invert_check.add_quality_assessor(&check_inverted, err);
    Mesquite::MeshDomainAssoc mesh_and_domain = Mesquite::MeshDomainAssoc(&mymesh, 0);
    q_invert_check.run_common(&mesh_and_domain, 0, 0, err);
    int inverted_elems = 0, inverted_samples = 0;
    check_inverted.get_inverted_element_count(inverted_elems, inverted_samples, err);
    cout.rdbuf(backup);
    fout.close();
    if (inverted_elems || inverted_samples)
        return false;
    return true;
}
