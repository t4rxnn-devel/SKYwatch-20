// Cryptographic Data Lineage & Audit Ledger Module
use std::collections::hash_map::DefaultHasher;
use std::hash::{Hash, Hasher};

pub struct AuditProvenanceLedger;

#[derive(Debug, Clone)]
pub struct ProvenanceNode {
    pub transaction_id: u64,
    pub operator_intent: String,
    pub state_hash: u64,
    pub previous_hash: u64,
}

impl AuditProvenanceLedger {
    /// Generates a cryptographically chained audit node for every state change
    pub fn Append_state_change(
        trans_id: u64,
        intent: &str,
        current_state_payload: &str,
        prev_hash: u64,
    ) -> ProvenanceNode {
        let mut hasher = DefaultHasher::new();
        trans_id.hash(&mut hasher);
        intent.hash(&mut hasher);
        current_state_payload.hash(&mut hasher);
        prev_hash.hash(&mut hasher);

        ProvenanceNode {
            transaction_id: trans_id,
            operator_intent: intent.to_string(),
            state_hash: hasher.finish(),
            previous_hash: prev_hash,
        }
    }
}
